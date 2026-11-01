#include "doctest.h"
#include "test_helpers.h"
#include "RoleBasedAssignment.h"
#include "LeastWorkloadAssignment.h"
#include "RuleBasedAssignment.h"
#include "WorkAllocator.h"

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
		CHECK(free.getWorkload() == before + 1);
	}
}

}
