// Thorough State tests: every (state x operation) pair, the state predicates, approvals, events,
// cloning, snapshots, and the state classes themselves.
#include <cstring>
#include "doctest.h"
#include "test_tree_helpers.h"
#include "AutoApprovalHandler.h"
#include "PendingState.h"
#include "AvailableState.h"
#include "AssignedState.h"
#include "InProgressState.h"
#include "AwaitingApprovalState.h"
#include "EscalatedState.h"
#include "CompletedState.h"
#include "RejectedState.h"
#include "WorkItemMemento.h"

using namespace testutil;

TEST_SUITE("State (thorough)") {

namespace {

AutoApprovalHandler gate(5);                  // only stored in a Task; the state machine never calls it

const char* const kStates[] = {"Pending", "Available", "Assigned", "InProgress",
                               "AwaitingApproval", "Escalated", "Completed", "Rejected"};
const char* const kOps[] = {"activate", "assign", "start", "complete", "reject", "escalate", "approve"};

// Builds an item and walks it into the requested state through the public API.
std::unique_ptr<BasicWorkItem> makeIn(const std::string& state, Participant& who, const char* id = "w") {
	std::shared_ptr<Task> t = spec("Task", "nurse");
	if (state == "AwaitingApproval") t->requireApproval("complete", &gate);
	std::unique_ptr<BasicWorkItem> w(new BasicWorkItem(id, t));
	if (state == "Pending") return w;
	w->activate();
	if (state == "Available") return w;
	w->assign(&who);
	if (state == "Assigned") return w;
	if (state == "Escalated") { w->escalate(); return w; }
	w->start();
	if (state == "InProgress") return w;
	if (state == "Completed" || state == "AwaitingApproval") { w->complete(); return w; }
	if (state == "Rejected") { w->reject("no"); return w; }
	throw std::logic_error("test bug: unknown state " + state);
}

void apply(BasicWorkItem& w, const std::string& op, Participant& other) {
	if (op == "activate") w.activate();
	else if (op == "assign") w.assign(&other);
	else if (op == "start") w.start();
	else if (op == "complete") w.complete();
	else if (op == "reject") w.reject("because");
	else if (op == "escalate") w.escalate();
	else if (op == "approve") w.approve();
	else throw std::logic_error("test bug: unknown op " + op);
}

// The whole state machine. "-" means the operation is illegal in that state.
// Columns follow kOps: activate assign start complete reject escalate approve
struct Row { const char* state; const char* result[7]; };
const Row kTable[] = {
	{"Pending",          {"Available",        "-",        "-",          "-",         "-",        "-",         "-"}},
	{"Available",        {"Available",        "Assigned", "-",          "-",         "-",        "Escalated", "-"}},
	{"Assigned",         {"Assigned",         "Assigned", "InProgress", "-",         "-",        "Escalated", "-"}},
	{"InProgress",       {"InProgress",       "-",        "-",          "Completed", "Rejected", "Escalated", "-"}},
	{"AwaitingApproval", {"AwaitingApproval", "-",        "-",          "-",         "Rejected", "-",         "Completed"}},
	{"Escalated",        {"Escalated",        "Assigned", "-",          "-",         "-",        "-",         "-"}},
	{"Completed",        {"Completed",        "-",        "-",          "-",         "-",        "-",         "-"}},
	{"Rejected",         {"Rejected",         "-",        "-",          "-",         "-",        "-",         "-"}},
};

} // namespace

// ================================================================== the whole machine

TEST_CASE("every operation in every state does exactly what the state machine says") {
	Participant ann("Ann", 1, roles("nurse"));
	Participant bob("Bob", 1, roles("nurse"));

	for (const Row& row : kTable) {
		for (int i = 0; i < 7; ++i) {
			const std::string from = row.state, op = kOps[i], expected = row.result[i];
			INFO("state " << from << ", operation " << op << ", expected " << expected);

			std::unique_ptr<BasicWorkItem> w = makeIn(from, ann);
			REQUIRE(w->getStateName() == from);
			WorkflowSubject subject; Recorder rec; subject.attach(&rec);
			w->bind(&subject);
			Participant* assigneeBefore = w->getAssignee();

			if (expected == "-") {
				CHECK_THROWS_AS(apply(*w, op, bob), std::logic_error);
				CHECK(w->getStateName() == from);           // a refused operation changes nothing...
				CHECK(w->getAssignee() == assigneeBefore);  // ...not the assignee...
				CHECK(rec.types.empty());                   // ...and announces nothing
			} else {
				CHECK_NOTHROW(apply(*w, op, bob));
				CHECK(w->getStateName() == expected);
				if (expected == from) {
					CHECK(rec.types.empty());               // staying put is silent
				} else {
					std::string lastChange;
					for (size_t k = 0; k < rec.types.size(); ++k)
						if (rec.types[k] == "state.changed") lastChange = rec.details[k];
					CHECK(lastChange == expected);          // the move was announced, naming the new state
				}
				if (op == "assign") CHECK(w->getAssignee() == &bob);
				else CHECK(w->getAssignee() == assigneeBefore);
			}
		}
	}
}

TEST_CASE("which states count as active and which as complete") {
	Participant ann("Ann", 1, roles("nurse"));
	struct Expect { const char* state; bool active; bool complete; };
	const Expect expect[] = {
		{"Pending", false, false}, {"Available", true, false}, {"Assigned", true, false},
		{"InProgress", true, false}, {"AwaitingApproval", true, false}, {"Escalated", true, false},
		{"Completed", false, true}, {"Rejected", false, false}};
	for (const Expect& e : expect) {
		INFO("state " << e.state);
		std::unique_ptr<BasicWorkItem> w = makeIn(e.state, ann);
		CHECK(w->isActive() == e.active);
		CHECK(w->isComplete() == e.complete);
		CHECK(w->isComplete() == (w->getStateName() == "Completed"));   // nothing but Completed is complete
	}
}

// ================================================================== paths through the machine

TEST_CASE("the happy path announces every step") {
	Participant ann("Ann", 1, roles("nurse"));
	BasicWorkItem w("w", spec("T", "nurse"));
	WorkflowSubject subject; Recorder rec; subject.attach(&rec);
	w.bind(&subject);
	work(w, ann);
	std::vector<std::string> want = {"Available", "Assigned", "InProgress", "Completed"};
	CHECK(rec.details == want);
	CHECK(rec.count("state.changed") == 4);
}

TEST_CASE("reassigning an assigned item swaps the person without a state change") {
	Participant ann("Ann", 1, roles("nurse")), bob("Bob", 1, roles("nurse"));
	BasicWorkItem w("w", spec("T", "nurse"));
	w.activate(); w.assign(&ann);
	WorkflowSubject subject; Recorder rec; subject.attach(&rec);
	w.bind(&subject);

	w.assign(&bob);
	CHECK(w.getStateName() == "Assigned");
	CHECK(w.getAssignee() == &bob);
	CHECK(rec.types.empty());
	w.start();                                       // and the new person carries on
	CHECK(w.getAssignee() == &bob);
}

TEST_CASE("escalation can be recovered from by reassigning") {
	Participant ann("Ann", 1, roles("nurse")), boss("Bea", 3, roles("nurse"));

	SUBCASE("escalated before it was started") {
		std::unique_ptr<BasicWorkItem> w = makeIn("Assigned", ann);
		w->escalate();
		CHECK(w->getStateName() == "Escalated");
		w->assign(&boss);
		CHECK(w->getStateName() == "Assigned");
		w->start(); w->complete();
		CHECK(w->isComplete());
	}
	SUBCASE("escalated mid-work") {
		std::unique_ptr<BasicWorkItem> w = makeIn("InProgress", ann);
		w->escalate();
		w->assign(&boss);
		CHECK(w->getStateName() == "Assigned");     // work restarts from Assigned, not InProgress
		CHECK_THROWS_AS(w->complete(), std::logic_error);
		w->start(); w->complete();
		CHECK(w->getAssignee() == &boss);
	}
	SUBCASE("escalated before anyone took it") {
		std::unique_ptr<BasicWorkItem> w = makeIn("Available", ann);
		w->escalate();
		CHECK(w->getStateName() == "Escalated");
		w->assign(&boss);
		CHECK(w->getStateName() == "Assigned");
	}
}

TEST_CASE("an item can be escalated more than once, with a reassignment in between") {
	Participant a("A", 1, roles("nurse")), b("B", 2, roles("nurse")), c("C", 3, roles("nurse"));
	std::unique_ptr<BasicWorkItem> w = makeIn("Assigned", a);
	w->escalate(); w->assign(&b);
	w->escalate(); w->assign(&c);
	w->start(); w->complete();
	CHECK(w->isComplete());
	CHECK(w->getAssignee() == &c);
}

TEST_CASE("assigning needs a person") {
	Participant ann("Ann", 1, roles("nurse"));
	for (const char* st : {"Available", "Assigned", "Escalated"}) {
		INFO("state " << st);
		std::unique_ptr<BasicWorkItem> w = makeIn(st, ann);
		Participant* before = w->getAssignee();
		CHECK_THROWS_AS(w->assign(nullptr), std::invalid_argument);
		CHECK(w->getStateName() == st);
		CHECK(w->getAssignee() == before);
	}
}

TEST_CASE("finished items stay finished however hard you push") {
	Participant ann("Ann", 1, roles("nurse"));
	for (const char* st : {"Completed", "Rejected"}) {
		INFO("state " << st);
		std::unique_ptr<BasicWorkItem> w = makeIn(st, ann);
		for (int round = 0; round < 3; ++round) {
			CHECK_THROWS_AS(w->assign(&ann), std::logic_error);
			CHECK_THROWS_AS(w->start(), std::logic_error);
			CHECK_THROWS_AS(w->complete(), std::logic_error);
			CHECK_THROWS_AS(w->reject("again"), std::logic_error);
			CHECK_THROWS_AS(w->escalate(), std::logic_error);
			CHECK_THROWS_AS(w->approve(), std::logic_error);
			CHECK_NOTHROW(w->activate());
		}
		CHECK(w->getStateName() == st);
	}
}

TEST_CASE("activate() is the only thing a Pending item accepts, and it is repeatable") {
	BasicWorkItem w("w", spec("T", "nurse"));
	w.activate();
	w.activate();
	w.activate();
	CHECK(w.getStateName() == "Available");
}

// ================================================================== approval

TEST_CASE("without an approval requirement, completing finishes the item") {
	Participant ann("Ann", 1, roles("nurse"));
	std::unique_ptr<BasicWorkItem> w = makeIn("InProgress", ann);
	w->complete();
	CHECK(w->getStateName() == "Completed");
}

TEST_CASE("with an approval requirement, completing waits for approval") {
	Participant ann("Ann", 1, roles("doctor"));
	std::shared_ptr<Task> t = spec("Discharge", "doctor");
	t->requireApproval("complete", &gate);
	BasicWorkItem w("w", t);
	WorkflowSubject subject; Recorder rec; subject.attach(&rec);
	w.bind(&subject);

	w.activate(); w.assign(&ann); w.start();
	rec.types.clear(); rec.details.clear(); rec.itemIds.clear();
	w.complete();
	CHECK(w.getStateName() == "AwaitingApproval");
	CHECK_FALSE(w.isComplete());
	CHECK(w.isActive());
	CHECK(rec.types.size() == 2);
	CHECK(rec.types[0] == "state.changed");
	CHECK(rec.types[1] == "approval.requested");

	w.approve();
	CHECK(w.getStateName() == "Completed");
	CHECK(w.isComplete());
	CHECK(rec.types.back() == "state.changed");
	CHECK(rec.details.back() == "Completed");
}

TEST_CASE("approving twice is refused: the item is already completed") {
	Participant ann("Ann", 1, roles("doctor"));
	std::unique_ptr<BasicWorkItem> w = makeIn("AwaitingApproval", ann);
	w->approve();
	CHECK_THROWS_AS(w->approve(), std::logic_error);
	CHECK(w->getStateName() == "Completed");
}

TEST_CASE("an approval can be turned down") {
	Participant ann("Ann", 1, roles("doctor"));
	std::unique_ptr<BasicWorkItem> w = makeIn("AwaitingApproval", ann);
	w->reject("not signed off");
	CHECK(w->getStateName() == "Rejected");
	CHECK_THROWS_AS(w->approve(), std::logic_error);
}

TEST_CASE("an item waiting for approval cannot be worked on any further") {
	Participant ann("Ann", 1, roles("doctor"));
	std::unique_ptr<BasicWorkItem> w = makeIn("AwaitingApproval", ann);
	CHECK_THROWS_AS(w->complete(), std::logic_error);       // no double-submitting
	CHECK_THROWS_AS(w->start(), std::logic_error);
	CHECK_THROWS_AS(w->escalate(), std::logic_error);
	CHECK(w->getStateName() == "AwaitingApproval");
}

TEST_CASE("the approval requirement is read when the item completes, not when it is created") {
	Participant ann("Ann", 1, roles("nurse"));
	std::shared_ptr<Task> t = spec("Late rule", "nurse");
	BasicWorkItem w("w", t);
	w.activate(); w.assign(&ann); w.start();
	t->requireApproval("complete", &gate);                  // the spec changes after work began
	w.complete();
	CHECK(w.getStateName() == "AwaitingApproval");
}

TEST_CASE("items that share a task spec go through approval independently") {
	Participant ann("Ann", 1, roles("doctor"));
	std::shared_ptr<Task> t = spec("Shared", "doctor");
	t->requireApproval("complete", &gate);
	BasicWorkItem a("a", t), b("b", t);
	for (BasicWorkItem* w : {&a, &b}) { w->activate(); w->assign(&ann); w->start(); w->complete(); }
	CHECK(a.getStateName() == "AwaitingApproval");
	CHECK(b.getStateName() == "AwaitingApproval");
	a.approve();
	CHECK(a.getStateName() == "Completed");
	CHECK(b.getStateName() == "AwaitingApproval");
}

TEST_CASE("approval gates the stage that contains the item") {
	Participant ann("Ann", 1, roles("doctor"));
	std::shared_ptr<Task> t = spec("Sign-off", "doctor");
	t->requireApproval("complete", &gate);
	auto stage = makeStage("s");
	auto item = std::make_shared<BasicWorkItem>("i", t);
	stage->add(item);
	item->activate(); item->assign(&ann); item->start(); item->complete();
	CHECK_FALSE(stage->isComplete());
	item->approve();
	CHECK(stage->isComplete());
}

TEST_CASE("escalated and rejected items keep their stage open") {
	Participant ann("Ann", 1, roles("nurse"));
	auto stage = makeStage("s", true);
	auto escalated = std::make_shared<BasicWorkItem>("e", spec("E", "nurse"));
	auto rejected = std::make_shared<BasicWorkItem>("r", spec("R", "nurse"));
	stage->add(escalated); stage->add(rejected);
	escalated->activate(); escalated->assign(&ann); escalated->escalate();
	rejected->activate(); rejected->assign(&ann); rejected->start(); rejected->reject("x");
	CHECK_FALSE(stage->isComplete());
	escalated->assign(&ann); escalated->start(); escalated->complete();
	CHECK_FALSE(stage->isComplete());                      // a rejected item can never finish
}

// ================================================================== events

TEST_CASE("an unbound item is silent, and binding switches where events go") {
	Participant ann("Ann", 1, roles("nurse"));
	BasicWorkItem w("w", spec("T", "nurse"));
	WorkflowSubject s1, s2; Recorder r1, r2;
	s1.attach(&r1); s2.attach(&r2);

	w.activate();                                          // unbound
	CHECK(r1.types.empty());
	w.bind(&s1);
	w.assign(&ann);
	CHECK(r1.count("state.changed") == 1);
	w.bind(&s2);
	w.start();
	CHECK(r1.count("state.changed") == 1);
	CHECK(r2.count("state.changed") == 1);
	w.bind(nullptr);
	w.complete();
	CHECK(r2.count("state.changed") == 1);
}

TEST_CASE("events name the item and the state it just entered") {
	Participant ann("Ann", 1, roles("nurse"));
	BasicWorkItem w("item-42", spec("T", "nurse"));
	WorkflowSubject subject; Recorder rec; subject.attach(&rec);
	w.bind(&subject);
	w.activate();
	REQUIRE(rec.types.size() == 1);
	CHECK(rec.types[0] == "state.changed");
	CHECK(rec.details[0] == "Available");
	CHECK(rec.itemIds[0] == "item-42");
}

TEST_CASE("restoring a snapshot is announced as its own kind of event") {
	Participant ann("Ann", 1, roles("nurse"));
	std::unique_ptr<BasicWorkItem> w = makeIn("Assigned", ann);
	auto snap = w->createMemento();
	w->start();
	WorkflowSubject subject; Recorder rec; subject.attach(&rec);
	w->bind(&subject);
	w->restore(*snap);
	REQUIRE(rec.types.size() == 1);
	CHECK(rec.types[0] == "state.restored");
	CHECK(rec.details[0] == "Assigned");
}

// ================================================================== cloning

TEST_CASE("a clone starts in the same state, with the same assignee and data") {
	Participant ann("Ann", 1, roles("nurse"));
	for (const char* st : kStates) {
		INFO("state " << st);
		std::unique_ptr<BasicWorkItem> w = makeIn(st, ann);
		w->setField("note", st);
		std::unique_ptr<BasicWorkItem> copy(w->clone());
		CHECK(copy.get() != w.get());
		CHECK(copy->getId() == "w");
		CHECK(copy->getStateName() == st);
		CHECK(copy->getAssignee() == w->getAssignee());
		CHECK(copy->getField("note") == st);
		CHECK(copy->isActive() == w->isActive());
		CHECK(copy->isComplete() == w->isComplete());
		CHECK(&copy->getTask() == &w->getTask());
	}
}

TEST_CASE("a clone moves on without dragging the original along") {
	Participant ann("Ann", 1, roles("nurse"));
	std::unique_ptr<BasicWorkItem> w = makeIn("InProgress", ann);
	std::unique_ptr<BasicWorkItem> copy(w->clone());
	copy->complete();
	CHECK(copy->getStateName() == "Completed");
	CHECK(w->getStateName() == "InProgress");
	w->reject("changed my mind");
	CHECK(copy->getStateName() == "Completed");
}

TEST_CASE("a clone of an item awaiting approval can be approved separately") {
	Participant ann("Ann", 1, roles("doctor"));
	std::unique_ptr<BasicWorkItem> w = makeIn("AwaitingApproval", ann);
	std::unique_ptr<BasicWorkItem> copy(w->clone());
	copy->approve();
	CHECK(copy->getStateName() == "Completed");
	CHECK(w->getStateName() == "AwaitingApproval");         // the original still has its own pending approval
	w->approve();
	CHECK(w->getStateName() == "Completed");
}

TEST_CASE("a clone is not bound to the original's subject") {
	Participant ann("Ann", 1, roles("nurse"));
	BasicWorkItem w("w", spec("T", "nurse"));
	WorkflowSubject subject; Recorder rec; subject.attach(&rec);
	w.bind(&subject);
	std::unique_ptr<BasicWorkItem> copy(w.clone());
	copy->activate(); copy->assign(&ann);
	CHECK(rec.types.empty());
	w.activate();
	CHECK(rec.count("state.changed") == 1);
}

// ================================================================== snapshots and state

TEST_CASE("a snapshot taken in any state restores that state, assignee and data") {
	Participant ann("Ann", 1, roles("nurse"));
	for (const char* st : kStates) {
		INFO("state " << st);
		std::unique_ptr<BasicWorkItem> src = makeIn(st, ann);
		src->setField("k", st);
		auto snap = src->createMemento();

		BasicWorkItem target("t", spec("Other", "nurse"));   // a different item, starting from Pending
		target.restore(*snap);
		CHECK(target.getStateName() == st);
		CHECK(target.getAssignee() == src->getAssignee());
		CHECK(target.getField("k") == st);
		CHECK(target.isComplete() == src->isComplete());
	}
}

TEST_CASE("a snapshot is frozen: later changes to the item do not leak into it") {
	Participant ann("Ann", 1, roles("nurse"));
	std::unique_ptr<BasicWorkItem> w = makeIn("Assigned", ann);
	w->setField("k", "before");
	auto snap = w->createMemento();
	w->start(); w->setField("k", "after"); w->complete();
	REQUIRE(w->isComplete());
	w->restore(*snap);
	CHECK(w->getStateName() == "Assigned");
	CHECK(w->getField("k") == "before");
}

TEST_CASE("a snapshot of an item awaiting approval can be restored and approved again and again") {
	Participant ann("Ann", 1, roles("doctor"));
	std::unique_ptr<BasicWorkItem> w = makeIn("AwaitingApproval", ann);
	auto snap = w->createMemento();
	for (int round = 0; round < 3; ++round) {
		INFO("round " << round);
		w->approve();
		REQUIRE(w->getStateName() == "Completed");
		w->restore(*snap);
		CHECK(w->getStateName() == "AwaitingApproval");
	}
}

TEST_CASE("a restored item carries on from the restored state") {
	Participant ann("Ann", 1, roles("nurse"));
	std::unique_ptr<BasicWorkItem> w = makeIn("Assigned", ann);
	auto snap = w->createMemento();
	w->start(); w->complete();
	w->restore(*snap);                                       // back from Completed to Assigned
	CHECK_THROWS_AS(w->complete(), std::logic_error);        // the rules of Assigned apply again
	w->start(); w->complete();
	CHECK(w->isComplete());
}

TEST_CASE("whether an item is active or complete follows the restored state") {
	Participant ann("Ann", 1, roles("nurse"));
	std::unique_ptr<BasicWorkItem> done = makeIn("Completed", ann);
	auto snap = done->createMemento();
	BasicWorkItem fresh("f", spec("F", "nurse"));
	CHECK_FALSE(fresh.isActive());
	fresh.restore(*snap);
	CHECK_FALSE(fresh.isActive());
	CHECK(fresh.isComplete());
}

// ================================================================== the state classes and BasicWorkItem plumbing

TEST_CASE("each state object reports its name, flags, and can clone itself") {
	struct Expect { std::unique_ptr<WorkItemState> state; const char* name; bool active; bool complete; };
	std::vector<Expect> all;
	all.push_back({std::unique_ptr<WorkItemState>(new PendingState()), "Pending", false, false});
	all.push_back({std::unique_ptr<WorkItemState>(new AvailableState()), "Available", true, false});
	all.push_back({std::unique_ptr<WorkItemState>(new AssignedState()), "Assigned", true, false});
	all.push_back({std::unique_ptr<WorkItemState>(new InProgressState()), "InProgress", true, false});
	all.push_back({std::unique_ptr<WorkItemState>(new AwaitingApprovalState(new CompletedState(), "complete")),
	               "AwaitingApproval", true, false});
	all.push_back({std::unique_ptr<WorkItemState>(new EscalatedState()), "Escalated", true, false});
	all.push_back({std::unique_ptr<WorkItemState>(new CompletedState()), "Completed", false, true});
	all.push_back({std::unique_ptr<WorkItemState>(new RejectedState()), "Rejected", false, false});

	for (const Expect& e : all) {
		INFO("state " << e.name);
		CHECK(e.state->getName() == e.name);
		CHECK(e.state->isActive() == e.active);
		CHECK(e.state->isComplete() == e.complete);
		std::unique_ptr<WorkItemState> copy(e.state->clone());
		CHECK(copy.get() != e.state.get());
		CHECK(copy->getName() == e.name);
		CHECK(copy->isActive() == e.active);
		CHECK(copy->isComplete() == e.complete);
		std::unique_ptr<WorkItemState> copyOfCopy(copy->clone());
		CHECK(copyOfCopy->getName() == e.name);
	}
}

TEST_CASE("AwaitingApprovalState needs somewhere to go once approved") {
	CHECK_THROWS_AS(AwaitingApprovalState(nullptr, "complete"), std::invalid_argument);
}

TEST_CASE("an item refuses a missing spec or a missing state") {
	CHECK_THROWS_AS(BasicWorkItem("x", std::shared_ptr<const Task>()), std::invalid_argument);

	Participant ann("Ann", 1, roles("nurse"));
	std::unique_ptr<BasicWorkItem> w = makeIn("Assigned", ann);
	CHECK_THROWS_AS(w->setState(nullptr), std::invalid_argument);
	CHECK_THROWS_AS(w->advanceTo(nullptr, "complete"), std::invalid_argument);
	CHECK(w->getStateName() == "Assigned");
}

TEST_CASE("setState() swaps the state directly and announces it") {
	BasicWorkItem w("w", spec("T", "nurse"));
	WorkflowSubject subject; Recorder rec; subject.attach(&rec);
	w.bind(&subject);
	w.setState(new CompletedState());
	CHECK(w.getStateName() == "Completed");
	CHECK(w.isComplete());
	REQUIRE(rec.types.size() == 1);
	CHECK(rec.details[0] == "Completed");
}

TEST_CASE("advanceTo() applies the approval rule for the trigger it is given") {
	std::shared_ptr<Task> t = spec("T", "nurse");
	t->requireApproval("complete", &gate);

	SUBCASE("the gated trigger waits for approval") {
		BasicWorkItem w("w", t);
		w.advanceTo(new CompletedState(), "complete");
		CHECK(w.getStateName() == "AwaitingApproval");
		w.approve();
		CHECK(w.getStateName() == "Completed");
	}
	SUBCASE("any other trigger moves straight on") {
		BasicWorkItem w("w", t);
		w.advanceTo(new CompletedState(), "something else");
		CHECK(w.getStateName() == "Completed");
	}
}

}
