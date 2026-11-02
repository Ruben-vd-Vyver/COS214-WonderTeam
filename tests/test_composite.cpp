#include "doctest.h"
#include "test_helpers.h"
#include "Stage.h"
#include "WorkItem.h"
#include "WorkflowInstance.h"

using namespace testutil;

TEST_SUITE("Composite") {

TEST_CASE("sequential and parallel stages open their children correctly") {
	std::unique_ptr<WorkflowDefinition> def(buildDefinition(admissionScript()));
	REQUIRE(def != nullptr);
	std::unique_ptr<WorkflowInstance> inst(def->createInstance("composite"));
	WorkflowNode* root = inst->getRoot();

	root->activate(); // sequential root: only the first unfinished child opens
	CHECK(inst->findItem("item-0")->getStateName() == "Available");
	CHECK(inst->findItem("item-1")->getStateName() == "Pending");

	Participant ann("Ann", 1, roles("nurse"));
	WorkItem* triage = inst->findItem("item-0");
	triage->assign(&ann);
	triage->start();
	triage->complete();
	root->activate();

	// the parallel stage opens BOTH of its tasks at once
	CHECK(inst->findItem("item-1")->isActive());
	CHECK(inst->findItem("item-2")->isActive());
	CHECK_FALSE(root->isComplete());
}

TEST_CASE("find() searches through nested stages") {
	std::unique_ptr<WorkflowDefinition> def(buildDefinition(admissionScript()));
	REQUIRE(def != nullptr);
	WorkItem* xray = def->getRoot()->find("item-2");
	REQUIRE(xray != nullptr);
	CHECK(xray->getName() == "X-ray");
	CHECK(def->getRoot()->find("nope") == nullptr);
}

TEST_CASE("adding a parent into its own child is refused") {
	Stage* a = new Stage("a", "A", false);
	Stage* b = new Stage("b", "B", false);
	a->add(b);
	CHECK_THROWS_AS(b->add(a), std::invalid_argument);
	delete a; // a owns b
}

}
