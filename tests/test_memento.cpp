#include "doctest.h"
#include "test_helpers.h"
#include "BasicWorkItem.h"
#include "WorkItemMemento.h"

using namespace testutil;

TEST_SUITE("Memento") {

TEST_CASE("restore brings back state, assignee and data") {
	Participant ann("Ann", 1, roles("nurse"));
	Participant bob("Bob", 1, roles("nurse"));
	BasicWorkItem w("m", spec("Triage", "nurse"));
	w.activate();
	std::unique_ptr<WorkItemMemento> unassigned = w.createMemento();
	w.assign(&ann);
	CHECK(ann.getWorkload() == 1);
	CHECK(bob.getWorkload() == 0);
	w.setField("note", "first");

	std::unique_ptr<WorkItemMemento> snap = w.createMemento();

	w.assign(&bob);
	CHECK(ann.getWorkload() == 0);
	CHECK(bob.getWorkload() == 1);
	w.start();
	w.setField("note", "changed");
	REQUIRE(w.getStateName() == "InProgress");

	w.restore(*snap);
	CHECK(w.getStateName() == "Assigned");
	CHECK(w.getAssignee() == &ann);
	CHECK(ann.getWorkload() == 1);
	CHECK(bob.getWorkload() == 0);
	CHECK_FALSE(bob.unassign(&w.getTask()));
	CHECK(w.getField("note") == "first");

	SUBCASE("the same snapshot can be restored again") {
		w.start();
		w.restore(*snap);
		CHECK(w.getStateName() == "Assigned");
		CHECK(ann.getWorkload() == 1);
		CHECK(bob.getWorkload() == 0);
		CHECK_FALSE(bob.unassign(&w.getTask()));
	}

	w.restore(*unassigned);
	CHECK(w.getAssignee() == nullptr);
	CHECK(ann.getWorkload() == 0);
	CHECK(bob.getWorkload() == 0);
	CHECK_FALSE(ann.unassign(&w.getTask()));
}

}
