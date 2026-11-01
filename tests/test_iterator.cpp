#include "doctest.h"
#include "test_helpers.h"

using namespace testutil;

TEST_SUITE("Iterator") {

TEST_CASE("walks the tree depth-first, top to bottom") {
	std::unique_ptr<WorkflowDefinition> def(buildDefinition(admissionScript()));
	REQUIRE(def != nullptr);
	CHECK(visitOrder(def->getRoot().get()) == "item-0 stage-1 item-1 item-2 item-3");
}

TEST_CASE("iterating a stage only visits that stage; a task has nothing below it") {
	std::unique_ptr<WorkflowDefinition> def(buildDefinition(admissionScript()));
	REQUIRE(def != nullptr);
	WorkflowNode* root = def->getRoot().get();
	CHECK(visitOrder(root->child(1)) == "item-1 item-2");
	CHECK(visitOrder(root->child(0)) == ""); // NullIterator
}

TEST_CASE("first() restarts and current() is null when done") {
	std::unique_ptr<WorkflowDefinition> def(buildDefinition(admissionScript()));
	REQUIRE(def != nullptr);
	WorkflowIterator* it = def->getRoot()->createIterator();
	int firstPass = 0, secondPass = 0;
	for (it->first(); !it->isDone(); it->next()) ++firstPass;
	for (it->first(); !it->isDone(); it->next()) ++secondPass;
	CHECK(firstPass == 5);
	CHECK(secondPass == 5);
	CHECK(it->current() == nullptr);
	delete it;
}

}
