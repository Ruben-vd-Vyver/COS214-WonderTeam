#include "doctest.h"
#include "test_helpers.h"
#include "WorkflowInstance.h"
#include "WorkItem.h"
#include "BasicWorkItem.h"
#include "PriorityItem.h"

using namespace testutil;

TEST_SUITE("Prototype") {

TEST_CASE("every instance is an independent clone of the definition") {
	std::unique_ptr<WorkflowDefinition> def(buildDefinition(admissionScript()));
	REQUIRE(def != nullptr);
	std::unique_ptr<WorkflowInstance> case1(def->createInstance("case-1"));
	std::unique_ptr<WorkflowInstance> case2(def->createInstance("case-2"));

	WorkItem* t1 = case1->findItem("item-0");
	WorkItem* t2 = case2->findItem("item-0");
	WorkItem* tDef = def->getRoot()->find("item-0");
	REQUIRE(t1 != nullptr);
	REQUIRE(t2 != nullptr);
	REQUIRE(tDef != nullptr);

	CHECK(t1 != t2);
	CHECK(t1 != tDef);
	CHECK(visitOrder(case1->getRoot()) == visitOrder(def->getRoot().get()));
	CHECK(&t1->getTask() == &tDef->getTask()); // the read-only Task spec is shared

	SUBCASE("running one case leaves the other case and the template alone") {
		Participant ann("Ann", 1, roles("nurse"));
		case1->getRoot()->activate();
		t1->assign(&ann);
		t1->start();
		CHECK(t1->getStateName() == "InProgress");
		CHECK(t2->getStateName() == "Pending");
		CHECK(t2->getAssignee() == nullptr);
		CHECK(tDef->getStateName() == "Pending");
	}
}

TEST_CASE("a decorated item clones as the same decorator") {
	std::unique_ptr<WorkItem> deco(new PriorityItem(new BasicWorkItem("p", spec("Urgent", "nurse")), 7));
	std::unique_ptr<WorkItem> copy(deco->clone());
	PriorityItem* pc = dynamic_cast<PriorityItem*>(copy.get());
	REQUIRE(pc != nullptr);
	CHECK(pc != deco.get());
	CHECK(pc->getPriority() == 7);
}

TEST_CASE("cloning mid-run copies the current state and assignee") {
	Participant ann("Ann", 1, roles("nurse"));
	BasicWorkItem w("w", spec("Triage", "nurse"));
	w.activate();
	w.assign(&ann);
	w.start();
	std::unique_ptr<WorkItem> copy(w.clone());
	CHECK(copy->getStateName() == "InProgress");
	CHECK(copy->getAssignee() == &ann);
}

}
