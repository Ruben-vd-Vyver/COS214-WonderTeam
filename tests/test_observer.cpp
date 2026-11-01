#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "test_helpers.h"
#include "WorkflowInstance.h"
#include "WorkItem.h"
#include "AssignCommand.h"
#include "AuditLog.h"
#include "ApprovalService.h"
#include "DeadlineMonitor.h"
#include "WorkflowObserver.h"

using namespace testutil;

//detaches itself from the instance during its own callback
class SelfDetachingObserver : public WorkflowObserver {
public:
    WorkflowInstance* instanceToDetachFrom;
    int eventCount;

    SelfDetachingObserver(WorkflowInstance* inst) : instanceToDetachFrom(inst), eventCount(0) {}

    void onEvent(const WorkflowEvent& event) override {
        eventCount++;
        if (instanceToDetachFrom) {
            instanceToDetachFrom->detach(this);
        }
    }
};

//triggers a secondary workflow event during event handling (tests re-entrancy / queueing)
class CascadingObserver : public WorkflowObserver {
public:
    WorkflowInstance* instance;
    int eventCount;

    CascadingObserver(WorkflowInstance* inst) : instance(inst), eventCount(0) {}

    void onEvent(const WorkflowEvent& event) override {
        eventCount++;
        //nested data change event
        if (event.getType() == "WorkflowStarted" && eventCount == 1) {
            instance->setData("cascade_key", "cascade_value");
        }
    }
};

TEST_SUITE("Observer") {

TEST_CASE("observers attached to an instance receive its events") {
	std::unique_ptr<WorkflowDefinition> def(buildDefinition(admissionScript()));
	REQUIRE(def != nullptr);
	std::unique_ptr<WorkflowInstance> inst(def->createInstance("obs"));
	AuditLog log;
	ApprovalService approvals;
	DeadlineMonitor deadlines;
	inst->attach(&log);
	inst->attach(&approvals);
	inst->attach(&deadlines);

	inst->start();
	CHECK(log.getEntries().size() == 1); //WorkflowStarted

	SUBCASE("a work item changing state is reported through its instance") {
		Participant ann("Ann", 1, roles("nurse"));
		inst->getRoot()->activate();
		std::size_t before = log.getEntries().size();
		inst->perform(new AssignCommand(inst->findItem("item-0"), &ann));
		CHECK(log.getEntries().size() > before);
	}

	SUBCASE("each observer reacts to its own kind of event") {
		inst->requestApproval(inst->findItem("item-0"));
		CHECK(approvals.getApprovalEvents().size() == 1);

		inst->setDeadline(inst->findItem("item-0"), 0);
		CHECK(log.getEntries().back().find("DeadlineSet") != std::string::npos);
	}

	SUBCASE("a detached observer gets no more events") {
		inst->detach(&log);
		std::size_t after = log.getEntries().size();
		inst->setData("ward", "B");
		CHECK(log.getEntries().size() == after);
	}
}
//WorkflowSubject robustness and edge cases
TEST_CASE("WorkflowSubject handles null pointers and duplicate attachments safely") {
	std::unique_ptr<WorkflowDefinition> def(buildDefinition(admissionScript()));
	REQUIRE(def != nullptr);
	std::unique_ptr<WorkflowInstance> inst(def->createInstance("obs-robust"));

	AuditLog log;
	//nullptr
	CHECK_NOTHROW(inst->attach(nullptr));
	CHECK_NOTHROW(inst->detach(nullptr));

	//Duplicate attachment check (should prevent duplicate entries)
	inst->attach(&log);
	inst->attach(&log); //Attempting duplicate add

	inst->start();
	CHECK(log.getEntries().size() == 1);// Exactly 1 entry= deduplication works

	//Multiple consecutive detaches should be safe
	CHECK_NOTHROW(inst->detach(&log));
	CHECK_NOTHROW(inst->detach(&log));
}
TEST_CASE("Workflow instances are completely isolated from each other's observers") {
	std::unique_ptr<WorkflowDefinition> def(buildDefinition(admissionScript()));
	REQUIRE(def != nullptr);

	std::unique_ptr<WorkflowInstance> inst1(def->createInstance("instance-1"));
	std::unique_ptr<WorkflowInstance> inst2(def->createInstance("instance-2"));

	AuditLog log1;
	AuditLog log2;

	inst1->attach(&log1);
	inst2->attach(&log2);

	inst1->start();

	//inst1's observer should capture the event
	//inst2's observer must remain untouched
	CHECK(log1.getEntries().size() == 1);
	CHECK(log2.getEntries().empty());
}
TEST_CASE("Observer can safely detach itself during event handling") {
	std::unique_ptr<WorkflowDefinition> def(buildDefinition(admissionScript()));
	REQUIRE(def != nullptr);
	std::unique_ptr<WorkflowInstance> inst(def->createInstance("obs-self-detach"));

	SelfDetachingObserver selfDetacher(inst.get());
	AuditLog log;

	inst->attach(&selfDetacher);
	inst->attach(&log);

	//First event: both receive it, and selfDetacher unsubscribes itself
	inst->start();
	CHECK(selfDetacher.eventCount == 1);
	CHECK(log.getEntries().size() == 1);

	//Second event: selfDetacher should receive NO further events
	inst->setData("room", "102");
	CHECK(selfDetacher.eventCount == 1); //Count should not increment
	CHECK(log.getEntries().size() == 2);   //AuditLog receives it
}
//Re-entrancy and event queueing (`pendingEvents` and `dispatching` flag)
TEST_CASE("Events triggered during dispatch are queued correctly without crashing") {
	std::unique_ptr<WorkflowDefinition> def(buildDefinition(admissionScript()));
	REQUIRE(def != nullptr);
	std::unique_ptr<WorkflowInstance> inst(def->createInstance("obs-cascade"));

	CascadingObserver cascader(inst.get());
	AuditLog log;

	inst->attach(&cascader);
	inst->attach(&log);

	inst->start();

	CHECK(cascader.eventCount >= 1);
	CHECK(log.getEntries().size() == 2); //Captured both WorkflowStarted and WorkflowDataUpdated
}
//DeadlineMonitor lifecycle behavior
TEST_CASE("DeadlineMonitor tracks, updates, and erases work item deadlines") {
	std::unique_ptr<WorkflowDefinition> def(buildDefinition(admissionScript()));
	REQUIRE(def != nullptr);
	std::unique_ptr<WorkflowInstance> inst(def->createInstance("obs-deadline"));

	DeadlineMonitor deadlines;
	inst->attach(&deadlines);

	inst->start();
	WorkItem* triageItem = inst->findItem("item-0");
	REQUIRE(triageItem != nullptr);

	//Set deadline
	std::time_t targetTime = std::time(nullptr) + 600;
	inst->setDeadline(triageItem, targetTime);

	//Completing the work item should prompt DeadlineMonitor to remove the deadline entry
	CHECK_NOTHROW(inst->completeWorkItem(triageItem));
}
}
