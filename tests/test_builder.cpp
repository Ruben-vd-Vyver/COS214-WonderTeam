#include "doctest.h"
#include "test_helpers.h"
#include "Stage.h"
#include "WorkItem.h"

using namespace testutil;

TEST_SUITE("Builder") {

TEST_CASE("the Director turns a script into the right workflow tree") {
	std::unique_ptr<WorkflowDefinition> def(buildDefinition(admissionScript()));
	REQUIRE(def != nullptr);

	WorkflowNode* root = def->getRoot().get();
	CHECK(root->childCount() == 3); // Triage, Tests stage, Discharge

	Stage* tests = dynamic_cast<Stage*>(root->child(1));
	REQUIRE(tests != nullptr);
	CHECK(tests->isParallel());
	CHECK(tests->childCount() == 2);

	CHECK(def->getTasks().size() == 4);
	REQUIRE(root->child(0)->asWorkItem() != nullptr);
	CHECK(root->child(0)->asWorkItem()->getRequiredRole() == "nurse");
}

TEST_CASE("an unknown script command makes construct() fail") {
	CHECK(buildDefinition("task | A | nurse\nfly | away\n") == nullptr);
}

TEST_CASE("getResult() refuses a definition with a stage still open") {
	DefinitionBuilder b;
	b.reset("Broken", 1);
	b.beginStage("Never closed", false);
	CHECK_THROWS_AS(b.getResult(), const char*);
}

}
