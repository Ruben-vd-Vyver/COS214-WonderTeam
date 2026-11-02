#include "doctest.h"
#include "test_helpers.h"
#include "TividyEngine.h"
#include "RoleBasedAssignment.h"
#include "AuditLog.h"

using namespace testutil;

TEST_SUITE("Facade") {

TEST_CASE("a whole case runs from deploy to completion through TividyEngine") {
	TividyEngine engine;
	Participant ann("Ann", 2, roles("nurse"));
	Participant ray("Ray", 2, roles("radiographer"));
	Participant doc("Doc", 5, roles("doctor"));
	engine.registerParticipant(&ann);
	engine.registerParticipant(&ray);
	engine.registerParticipant(&doc);

	std::istringstream script(admissionScript());
	WorkflowDirector director(script);
	std::string defId = engine.deploy(director, "Admission", 1);
	CHECK(defId == "Admission-v1");

	AuditLog log;
	std::string id = engine.startInstance(defId, std::vector<WorkflowObserver*>(1, &log));
	RoleBasedAssignment byRole;
	engine.setAssignment(id, &byRole);
	CHECK_FALSE(log.getEntries().empty());

	engine.assignNext(id, "item-0");
	CHECK_THROWS_AS(engine.startItem(id, "item-0", "Ray"), std::runtime_error); // not the assignee
	engine.startItem(id, "item-0", "Ann");
	engine.completeItem(id, "item-0", "Ann");

	engine.assignNext(id, "item-1"); engine.startItem(id, "item-1", "Ann"); engine.completeItem(id, "item-1", "Ann");
	engine.assignNext(id, "item-2"); engine.startItem(id, "item-2", "Ray"); engine.completeItem(id, "item-2", "Ray");
	engine.assignNext(id, "item-3"); engine.startItem(id, "item-3", "Doc");
	CHECK(engine.undoLast(id));
	engine.startItem(id, "item-3", "Doc");
	engine.completeItem(id, "item-3", "Doc");

	std::string report = engine.statusReport(id);
	MESSAGE(report);
	CHECK(report.find("COMPLETE") != std::string::npos);
	CHECK(report.find("4/4") != std::string::npos);

	SUBCASE("a second case starts fresh") {
		std::string id2 = engine.startInstance(defId);
		CHECK(engine.statusReport(id2).find("0/4") != std::string::npos);
	}
}

TEST_CASE("unknown ids are reported as errors") {
	TividyEngine engine;
	CHECK_THROWS_AS(engine.startInstance("nope-v1"), std::invalid_argument);
	CHECK_THROWS_AS(engine.statusReport("instance-99"), std::invalid_argument);
}

TEST_CASE("facade assignment, reassignment, and undo update participant workload") {
	TividyEngine engine;
	Participant ann("Ann", 1, roles("nurse"));
	Participant bob("Bob", 1, roles("nurse"));
	engine.registerParticipant(&ann);
	engine.registerParticipant(&bob);

	std::istringstream script(admissionScript());
	WorkflowDirector director(script);
	const std::string definitionId = engine.deploy(director, "Admission", 1);
	const std::string instanceId = engine.startInstance(definitionId);

	const auto assigneeOfTriage = [&engine, &instanceId]() {
		const std::vector<TividyEngine::WorkflowEntry> entries = engine.workflowTree(instanceId);
		for (std::size_t i = 0; i < entries.size(); ++i)
			if (entries[i].id == "item-0")
				return entries[i].assignee;
		return std::string();
	};

	engine.assignItem(instanceId, "item-0", "Ann");
	CHECK(ann.getWorkload() == 1);
	CHECK(bob.getWorkload() == 0);
	CHECK(assigneeOfTriage() == "Ann");

	engine.assignItem(instanceId, "item-0", "Bob");
	CHECK(ann.getWorkload() == 0);
	CHECK(bob.getWorkload() == 1);
	CHECK(assigneeOfTriage() == "Bob");

	CHECK(engine.undoLast(instanceId));
	CHECK(ann.getWorkload() == 1);
	CHECK(bob.getWorkload() == 0);
	CHECK(assigneeOfTriage() == "Ann");

	CHECK(engine.undoLast(instanceId));
	CHECK(ann.getWorkload() == 0);
	CHECK(bob.getWorkload() == 0);
	CHECK(assigneeOfTriage() == "unassigned");
}

}
