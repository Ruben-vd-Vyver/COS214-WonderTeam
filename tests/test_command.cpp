#include "doctest.h"
#include "test_helpers.h"
#include "WorkflowInstance.h"
#include "WorkItem.h"
#include "AssignCommand.h"
#include "StartCommand.h"
#include "CompleteCommand.h"

using namespace testutil;

TEST_SUITE("Command") {

TEST_CASE("commands run through the instance and undo in reverse order") {
	std::unique_ptr<WorkflowDefinition> def(buildDefinition(admissionScript()));
	REQUIRE(def != nullptr);
	std::unique_ptr<WorkflowInstance> inst(def->createInstance("cmd"));
	Participant ann("Ann", 1, roles("nurse"));
	inst->getRoot()->activate();
	WorkItem* item = inst->findItem("item-0");
	REQUIRE(item != nullptr);

	CHECK(inst->perform(new AssignCommand(item, &ann)));
	CHECK(inst->perform(new StartCommand(item)));
	CHECK(item->getStateName() == "InProgress");

	CHECK(inst->undoLast());
	CHECK(item->getStateName() == "Assigned");

	CHECK(inst->undoLast());
	CHECK(item->getStateName() == "Available");
	CHECK(item->getAssignee() == nullptr);

	CHECK_FALSE(inst->undoLast()); // nothing left
}

TEST_CASE("an illegal command is refused and not added to the history") {
	std::unique_ptr<WorkflowDefinition> def(buildDefinition(admissionScript()));
	REQUIRE(def != nullptr);
	std::unique_ptr<WorkflowInstance> inst(def->createInstance("cmd"));
	inst->getRoot()->activate();
	WorkItem* item = inst->findItem("item-0");
	REQUIRE(item != nullptr);

	CHECK_THROWS_AS(inst->perform(new CompleteCommand(item)), std::logic_error);
	CHECK_FALSE(inst->undoLast());
	CHECK(item->getStateName() == "Available");
}

}
