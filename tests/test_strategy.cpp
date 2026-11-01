#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "test_helpers.h"
#include "RoleBasedAssignment.h"
#include "LeastWorkloadAssignment.h"
#include "RuleBasedAssignment.h"
#include "WorkAllocator.h"
#include "Participant.h"
#include "Task.h"

using namespace testutil;

TEST_SUITE("Strategy") {

TEST_CASE("each assignment strategy picks a different person") {
	Participant busy("Busy nurse", 1, roles("nurse"));
	Participant free("Free nurse", 1, roles("nurse"));
	Participant porter("Porter", 1, roles("porter"));
	for (int i = 0; i < 5; ++i) busy.incrementWorkload();
	std::vector<Participant*> pool;
	pool.push_back(&busy);
	pool.push_back(&porter);
	pool.push_back(&free);
	Task triage("Triage", "nurse");

	RoleBasedAssignment role;
	LeastWorkloadAssignment least;
	RuleBasedAssignment rule;
	CHECK(role.select(triage, pool) == &busy);    // first with the role
	CHECK(least.select(triage, pool) == &porter); // lowest workload, any role
	CHECK(rule.select(triage, pool) == &free);    // role AND workload under 5

	SUBCASE("the allocator's strategy can be swapped at runtime") {
		WorkAllocator alloc;
		CHECK(alloc.allocate(triage, pool) == nullptr); // no strategy yet
		alloc.setStrategy(&role);
		CHECK(alloc.allocate(triage, pool) == &busy);
		alloc.setStrategy(&rule);
		int before = free.getWorkload();
		CHECK(alloc.allocate(triage, pool) == &free);
		CHECK(free.getWorkload() == before);
	}
}
//Empty Pool Edge Cases
TEST_CASE("strategies handle empty participant pools safely") {
	std::vector<Participant*> emptyPool;
	Task triage("Triage", "nurse");

	RoleBasedAssignment roleStrategy;
	LeastWorkloadAssignment leastStrategy;
	RuleBasedAssignment ruleStrategy;

	CHECK(roleStrategy.select(triage, emptyPool) == nullptr);
	CHECK(leastStrategy.select(triage, emptyPool) == nullptr);
	CHECK(ruleStrategy.select(triage, emptyPool) == nullptr);
}

//No Matching Criteria Edge Cases
TEST_CASE("strategies return nullptr when no participants match constraints") {
	Participant seniorDoctor("Dr. House", 1, roles("doctor"));
	Participant overworkedNurse("Nurse Ratched", 1, roles("nurse"));
	
	//Push workload up to 5 or higher for the rule-based check
	for (int i = 0; i < 6; ++i) {
		overworkedNurse.incrementWorkload();
	}

	std::vector<Participant*> pool;
	pool.push_back(&seniorDoctor);
	pool.push_back(&overworkedNurse);

	Task triage("Triage", "nurse"); //Requires "nurse"

	RoleBasedAssignment roleStrategy;
	RuleBasedAssignment ruleStrategy;

	//Role-based works because overworkedNurse has the role
	CHECK(roleStrategy.select(triage, pool) == &overworkedNurse);

	//Rule-based should fail because overworkedNurse's workload is 6 (not < 5)
	CHECK(ruleStrategy.select(triage, pool) == nullptr);

	//Task requiring an unrepresented role ("radiographer")
	Task xray("X-ray", "radiographer");
	CHECK(roleStrategy.select(xray, pool) == nullptr);
	CHECK(ruleStrategy.select(xray, pool) == nullptr);
}
//WorkAllocator Context & Runtime Swapping
TEST_CASE("WorkAllocator selects participants without mutating their workloads") {
	Participant nurse1("Alice", 1, roles("nurse"));
	Participant nurse2("Bob", 1, roles("nurse"));

	std::vector<Participant*> pool = {&nurse1, &nurse2};
	Task triage("Triage", "nurse");

	WorkAllocator alloc;

	//Behavior with no strategy set
	CHECK(alloc.allocate(triage, pool) == nullptr);

	SUBCASE("Swapping to RoleBasedAssignment") {
		RoleBasedAssignment roleStrat;
		alloc.setStrategy(&roleStrat);

		Participant* selected = alloc.allocate(triage, pool);
		CHECK(selected == &nurse1);
		CHECK(nurse1.getWorkload() == 0);
	}

	SUBCASE("Dynamic strategy swapping and cumulative workload tracking") {
		RoleBasedAssignment roleStrat;
		LeastWorkloadAssignment leastStrat;

		//nurse1 workload = 2, nurse2 workload = 0
		nurse1.incrementWorkload();
		nurse1.incrementWorkload();

		//Use least workload strategy
		alloc.setStrategy(&leastStrat);
		Participant* selected = alloc.allocate(triage, pool);
		
		//Should pick nurse2 because workload (0) < nurse1 workload (2)
		CHECK(selected == &nurse2);
		CHECK(nurse2.getWorkload() == 0);

		//Swap back to role-based
		alloc.setStrategy(&roleStrat);
		selected = alloc.allocate(triage, pool);
		CHECK(selected == &nurse1);
		CHECK(nurse1.getWorkload() == 2);
	}
}
}
