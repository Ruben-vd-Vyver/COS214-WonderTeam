#include "doctest.h"
#include "test_helpers.h"
#include "BasicWorkItem.h"
#include "AutoApprovalHandler.h"

using namespace testutil;

TEST_SUITE("State") {

TEST_CASE("normal lifecycle: Pending -> Available -> Assigned -> InProgress -> Completed") {
	Participant ann("Ann", 1, roles("nurse"));
	BasicWorkItem w("w", spec("Triage", "nurse"));
	CHECK(w.getStateName() == "Pending");
	w.activate();
	CHECK(w.getStateName() == "Available");
	w.assign(&ann);
	CHECK(w.getStateName() == "Assigned");
	w.start();
	CHECK(w.getStateName() == "InProgress");
	w.complete();
	CHECK(w.getStateName() == "Completed");
	CHECK(w.isComplete());
}

TEST_CASE("invalid transitions are refused") {
	Participant ann("Ann", 1, roles("nurse"));
	BasicWorkItem w("w", spec("Triage", "nurse"));
	CHECK_THROWS_AS(w.start(), std::logic_error); // still Pending
	w.activate();
	w.assign(&ann);
	w.start();
	w.complete();
	CHECK_THROWS_AS(w.reject("too late"), std::logic_error); // already Completed
}

TEST_CASE("reject and escalate") {
	Participant ann("Ann", 1, roles("nurse"));

	BasicWorkItem r("r", spec("X-ray", "radiographer"));
	r.activate(); r.assign(&ann); r.start();
	r.reject("wrong patient");
	CHECK(r.getStateName() == "Rejected");

	BasicWorkItem e("e", spec("Surgery", "doctor"));
	e.activate(); e.assign(&ann);
	e.escalate();
	CHECK(e.getStateName() == "Escalated");
	e.assign(&ann);
	CHECK(e.getStateName() == "Assigned");
}

TEST_CASE("an item that needs approval waits in AwaitingApproval") {
	Participant ann("Ann", 1, roles("doctor"));
	AutoApprovalHandler chain(5);
	std::shared_ptr<Task> needsSignOff = spec("Discharge", "doctor");
	needsSignOff->requireApproval("complete", &chain);

	BasicWorkItem a("a", needsSignOff);
	a.activate(); a.assign(&ann); a.start();
	a.complete();
	CHECK(a.getStateName() == "AwaitingApproval");
	a.approve();
	CHECK(a.getStateName() == "Completed");
}

}
