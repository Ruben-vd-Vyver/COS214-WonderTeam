#include "doctest.h"
#include "test_helpers.h"
#include "BasicWorkItem.h"
#include "PriorityItem.h"
#include "ValidatedItem.h"
#include "SecuredItem.h"

using namespace testutil;

TEST_SUITE("Decorator") {

TEST_CASE("PriorityItem passes assign() through and records its priority") {
	Participant ann("Ann", 1, roles("nurse"));
	PriorityItem prio(new BasicWorkItem("p", spec("Urgent", "nurse")), 9);
	prio.activate();
	prio.assign(&ann);
	CHECK(prio.getAssignee() == &ann);
	CHECK(prio.getField("priority") == "9");
}

TEST_CASE("ValidatedItem blocks complete() until required fields are filled") {
	Participant ann("Ann", 1, roles("nurse"));
	std::shared_ptr<Task> form = spec("Admission form", "nurse");
	form->addRequiredField("patientId");
	ValidatedItem valid(new BasicWorkItem("v", form));
	valid.activate();
	valid.assign(&ann);
	valid.start();

	CHECK_THROWS_AS(valid.complete(), std::runtime_error);
	valid.setField("patientId", "P-001");
	valid.complete();
	CHECK(valid.isComplete());
}

TEST_CASE("SecuredItem only accepts people with a permitted role") {
	Participant ann("Ann", 1, roles("nurse"));
	Participant bob("Bob", 1, roles("porter"));
	SecuredItem secure(new BasicWorkItem("s", spec("Dispense", "nurse")), roles("nurse"));
	secure.activate();
	CHECK_THROWS_AS(secure.assign(&bob), std::runtime_error);
	secure.assign(&ann);
	CHECK(secure.getAssignee() == &ann);
}

TEST_CASE("decorators stack") {
	Participant ann("Ann", 1, roles("nurse"));
	SecuredItem stacked(new PriorityItem(new BasicWorkItem("x", spec("Both", "nurse")), 3), roles("nurse"));
	stacked.activate();
	stacked.assign(&ann);
	CHECK(stacked.getAssignee() == &ann);       // Secured let it through
	CHECK(stacked.getField("priority") == "3"); // Priority still acted
}

}
