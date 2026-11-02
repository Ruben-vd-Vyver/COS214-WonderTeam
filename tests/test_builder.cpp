#include "doctest.h"
#include "test_helpers.h"
#include "Stage.h"
#include "WorkItem.h"
#include "AutoApprovalHandler.h"

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

TEST_CASE("approval definitions accept explicit levels and default old syntax to level 1") {
	AutoApprovalHandler chain(1);
	std::istringstream input(
		"workflow | Approval | 1\n"
		"task | Build Game | Developer\n"
		"approve | complete | directorChain | 3\n"
		"task | Package Game | Release Manager\n"
		"approve | complete | directorChain\n");
	WorkflowDirector director(input);
	director.registerApprovalChain("directorChain", &chain);

	DefinitionBuilder builder;
	REQUIRE(director.construct(builder));
	std::unique_ptr<WorkflowDefinition> definition(builder.getResult());

	REQUIRE(definition->getTasks().size() == 2);
	CHECK(definition->getTasks()[0]->getRequiredLevel("complete") == 3);
	CHECK(definition->getTasks()[1]->getRequiredLevel("complete") == 1);
}

TEST_CASE("approval definitions reject invalid required levels") {
	AutoApprovalHandler chain(1);
	const std::vector<std::string> invalidLevels = {
		"-1",
		"2147483648",
		"1.5"
	};

	for (std::size_t i = 0; i < invalidLevels.size(); ++i) {
		std::istringstream input(
			"workflow | Approval | 1\n"
			"task | Build Game | Developer\n"
			"approve | complete | directorChain | " + invalidLevels[i] + "\n");
		WorkflowDirector director(input);
		director.registerApprovalChain("directorChain", &chain);
		DefinitionBuilder builder;

		CHECK_FALSE(director.construct(builder));
		CHECK(director.getError().find("required approval level") != std::string::npos);
	}
}

}
