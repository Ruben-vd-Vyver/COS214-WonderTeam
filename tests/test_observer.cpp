#include "doctest.h"
#include "test_helpers.h"
#include "WorkflowInstance.h"
#include "WorkItem.h"
#include "AssignCommand.h"
#include "AuditLog.h"
#include "ApprovalService.h"
#include "DeadlineMonitor.h"

using namespace testutil;

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
	CHECK(log.getEntries().size() == 1); // WorkflowStarted

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

}
