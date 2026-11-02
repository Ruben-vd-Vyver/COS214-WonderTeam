// Thorough Builder tests: the DefinitionBuilder, the product (WorkflowDefinition), and the Director.
#include "doctest.h"
#include "test_build_helpers.h"
#include "AutoApprovalHandler.h"
#include "PriorityItem.h"
#include "WorkflowInstance.h"

using namespace testutil;

TEST_SUITE("Builder (thorough)") {

namespace {
std::unique_ptr<WorkflowDefinition> finish(DefinitionBuilder& b) {
	return std::unique_ptr<WorkflowDefinition>(b.getResult());
}
}

// ================================================================== DefinitionBuilder: lifecycle

TEST_CASE("a builder that has not been reset refuses every operation") {
	DefinitionBuilder b;
	AutoApprovalHandler chain(5);
	CHECK_THROWS_AS(b.beginStage("S", false), const char*);
	CHECK_THROWS_AS(b.endStage(), const char*);
	CHECK_THROWS_AS(b.addTask("a", "r"), const char*);
	CHECK_THROWS_AS(b.addCustomItem(makeItem("x")), const char*);
	CHECK_THROWS_AS(b.requireApproval("complete", &chain), const char*);
	CHECK_THROWS_AS(b.getResult(), const char*);
}

TEST_CASE("reset() starts a definition with an empty, sequential root stage") {
	DefinitionBuilder b;
	b.reset("Admission", 7);
	std::unique_ptr<WorkflowDefinition> def = finish(b);

	CHECK(def->getName() == "Admission");
	CHECK(def->getVersion() == 7);
	REQUIRE(def->getRoot() != nullptr);
	Stage* root = dynamic_cast<Stage*>(def->getRoot().get());
	REQUIRE(root != nullptr);
	CHECK_FALSE(root->isParallel());
	CHECK(root->getName() == "Admission");
	CHECK(root->getId() == "stage-0");
	CHECK(root->childCount() == 0);
	CHECK(def->getTasks().empty());
	CHECK(def->getChains().empty());
}

TEST_CASE("reset() throws away unfinished work and restarts the numbering") {
	DefinitionBuilder b;
	b.reset("First", 1);
	b.addTask("old", "r");
	b.beginStage("never closed", true);          // left open on purpose

	b.reset("Second", 2);
	b.addTask("fresh", "r");
	std::unique_ptr<WorkflowDefinition> def = finish(b);   // would throw if the old open stage survived

	CHECK(def->getName() == "Second");
	CHECK(def->getVersion() == 2);
	CHECK(def->getRoot()->childCount() == 1);
	CHECK(def->getRoot()->child(0)->getId() == "item-0");
	CHECK(def->getTasks().size() == 1);
}

TEST_CASE("one builder can produce several independent definitions") {
	DefinitionBuilder b;
	b.reset("One", 1);
	b.addTask("a", "r");
	b.addTask("b", "r");
	std::unique_ptr<WorkflowDefinition> one = finish(b);

	b.reset("Two", 1);
	b.addTask("c", "r");
	std::unique_ptr<WorkflowDefinition> two = finish(b);

	CHECK(one->getRoot().get() != two->getRoot().get());
	CHECK(visitOrder(one->getRoot().get()) == "item-0 item-1");
	CHECK(visitOrder(two->getRoot().get()) == "item-0");
	CHECK(one->getTasks().size() == 2);
	CHECK(two->getTasks().size() == 1);
	CHECK(one->getTasks()[0] != two->getTasks()[0]);
}

TEST_CASE("getResult() hands over the product and spends the builder") {
	DefinitionBuilder b;
	b.reset("W", 1);
	b.addTask("a", "r");
	std::unique_ptr<WorkflowDefinition> def = finish(b);
	REQUIRE(def != nullptr);

	CHECK_THROWS_AS(b.getResult(), const char*);        // nothing left to give
	CHECK_THROWS_AS(b.addTask("late", "r"), const char*);
	CHECK_THROWS_AS(b.beginStage("late", false), const char*);
	CHECK(def->getRoot()->childCount() == 1);           // the finished product is untouched

	b.reset("Again", 1);                                // reset() revives it
	CHECK_NOTHROW(b.addTask("fine", "r"));
}

TEST_CASE("the product does not depend on the builder staying alive") {
	std::unique_ptr<WorkflowDefinition> def;
	{
		DefinitionBuilder b;
		b.reset("W", 1);
		b.beginStage("S", true);
		b.addTask("a", "r");
		b.endStage();
		def = finish(b);
	}                                                   // builder destroyed here
	CHECK(visitOrder(def->getRoot().get()) == "stage-1 item-0");
	CHECK(def->getRoot()->find("item-0") != nullptr);
	CHECK(def->getTasks()[0]->getName() == "a");
}

// ================================================================== DefinitionBuilder: structure

TEST_CASE("ids follow creation order across every nesting level") {
	DefinitionBuilder b;
	b.reset("W", 1);
	b.addTask("a", "r");                    // item-0
	b.beginStage("S", false);               // stage-1
	b.addTask("b", "r");                    // item-1
	b.beginStage("T", true);                // stage-2
	b.addTask("c", "r");                    // item-2
	b.endStage();
	b.endStage();
	b.addTask("d", "r");                    // item-3
	std::unique_ptr<WorkflowDefinition> def = finish(b);

	CHECK(visitOrder(def->getRoot().get()) == "item-0 stage-1 item-1 stage-2 item-2 item-3");
	CHECK(def->getRoot()->child(1)->getName() == "S");
	CHECK(def->getRoot()->child(1)->child(1)->getName() == "T");
}

TEST_CASE("endStage() returns to the parent, so later tasks land in the right stage") {
	DefinitionBuilder b;
	b.reset("W", 1);
	b.addTask("a", "r");
	b.beginStage("par", true);
	b.addTask("b", "r");
	b.beginStage("inner", false);
	b.addTask("c", "r");
	b.endStage();
	b.addTask("d", "r");                    // back in "par"
	b.endStage();
	b.addTask("e", "r");                    // back in the root
	std::unique_ptr<WorkflowDefinition> def = finish(b);
	WorkflowNode* root = def->getRoot().get();

	REQUIRE(root->childCount() == 3);
	WorkflowNode* par = root->child(1);
	REQUIRE(par->childCount() == 3);
	CHECK(par->child(0)->getId() == "item-1");
	CHECK(par->child(1)->getName() == "inner");
	CHECK(par->child(1)->childCount() == 1);
	CHECK(par->child(2)->getId() == "item-3");
	CHECK(root->child(2)->getId() == "item-4");

	CHECK(dynamic_cast<Stage*>(par)->isParallel());
	CHECK_FALSE(dynamic_cast<Stage*>(par->child(1))->isParallel());
	CHECK_FALSE(dynamic_cast<Stage*>(root)->isParallel());
}

TEST_CASE("an empty stage is allowed and stays incomplete") {
	DefinitionBuilder b;
	b.reset("W", 1);
	b.beginStage("empty", false);
	b.endStage();
	std::unique_ptr<WorkflowDefinition> def = finish(b);
	REQUIRE(def->getRoot()->childCount() == 1);
	CHECK(def->getRoot()->child(0)->childCount() == 0);
	CHECK_FALSE(def->getRoot()->child(0)->isComplete());
}

TEST_CASE("two sibling stages are independent") {
	DefinitionBuilder b;
	b.reset("W", 1);
	b.beginStage("first", true);   b.addTask("a", "r");  b.addTask("b", "r");  b.endStage();
	b.beginStage("second", false); b.addTask("c", "r");  b.endStage();
	std::unique_ptr<WorkflowDefinition> def = finish(b);
	WorkflowNode* root = def->getRoot().get();
	CHECK(root->child(0)->childCount() == 2);
	CHECK(root->child(1)->childCount() == 1);
	CHECK(dynamic_cast<Stage*>(root->child(0))->isParallel());
	CHECK_FALSE(dynamic_cast<Stage*>(root->child(1))->isParallel());
}

TEST_CASE("deep nesting builds, closes and clones") {
	DefinitionBuilder b;
	b.reset("Deep", 1);
	const int depth = 100;
	for (int i = 0; i < depth; ++i) b.beginStage("level " + std::to_string(i), i % 2 == 0);
	b.addTask("bottom", "r");
	for (int i = 0; i < depth; ++i) b.endStage();
	b.addTask("top", "r");
	std::unique_ptr<WorkflowDefinition> def = finish(b);

	WorkflowNode* n = def->getRoot().get();
	for (int i = 0; i <= depth; ++i) { REQUIRE(n->childCount() >= 1); n = n->child(0); }   // 100 stages, then the task
	REQUIRE(n->asWorkItem() != nullptr);
	CHECK(n->getName() == "bottom");
	CHECK(def->getRoot()->childCount() == 2);
	std::unique_ptr<WorkflowNode> copy(def->getRoot()->clone());
	CHECK(visitOrder(copy.get()) == visitOrder(def->getRoot().get()));
}

// ================================================================== DefinitionBuilder: stage errors

TEST_CASE("endStage() with nothing open is refused and does not corrupt the builder") {
	DefinitionBuilder b;
	b.reset("W", 1);
	CHECK_THROWS_AS(b.endStage(), const char*);         // only the root is open
	b.beginStage("S", false);
	b.endStage();
	CHECK_THROWS_AS(b.endStage(), const char*);         // one too many
	CHECK_NOTHROW(b.addTask("still fine", "r"));
	std::unique_ptr<WorkflowDefinition> def = finish(b);
	CHECK(def->getRoot()->childCount() == 2);
}

TEST_CASE("getResult() refuses a half-built tree but the builder can carry on") {
	DefinitionBuilder b;
	b.reset("W", 1);
	b.beginStage("outer", false);
	b.beginStage("inner", false);
	CHECK_THROWS_AS(b.getResult(), const char*);        // two stages open
	b.endStage();
	CHECK_THROWS_AS(b.getResult(), const char*);        // one still open
	b.endStage();
	std::unique_ptr<WorkflowDefinition> def;
	CHECK_NOTHROW(def.reset(b.getResult()));            // the failed attempts did not spend the product
	REQUIRE(def != nullptr);
	CHECK(visitOrder(def->getRoot().get()) == "stage-1 stage-2");
}

// ================================================================== DefinitionBuilder: tasks

TEST_CASE("addTask() makes one spec per task, shared with its work item") {
	DefinitionBuilder b;
	b.reset("W", 1);
	b.addTask("Triage", "nurse");
	b.beginStage("S", true);
	b.addTask("X-ray", "radiographer");
	b.endStage();
	b.addTask("Triage", "nurse");           // same text again: a different task
	std::unique_ptr<WorkflowDefinition> def = finish(b);
	const auto& tasks = def->getTasks();

	REQUIRE(tasks.size() == 3);
	CHECK(tasks[0]->getName() == "Triage");
	CHECK(tasks[0]->getRequiredRole() == "nurse");
	CHECK(tasks[1]->getName() == "X-ray");
	CHECK(tasks[1]->getRequiredRole() == "radiographer");
	CHECK(tasks[0] != tasks[2]);

	WorkItem* first = def->getRoot()->find("item-0");
	WorkItem* xray = def->getRoot()->find("item-1");
	WorkItem* second = def->getRoot()->find("item-2");
	REQUIRE(first != nullptr); REQUIRE(xray != nullptr); REQUIRE(second != nullptr);
	CHECK(&first->getTask() == tasks[0].get());          // the item points at the registered spec, not a copy
	CHECK(&xray->getTask() == tasks[1].get());
	CHECK(&second->getTask() == tasks[2].get());
	CHECK(first->getName() == "Triage");
	CHECK(xray->getRequiredRole() == "radiographer");
	CHECK(first->getStateName() == "Pending");
	CHECK(first->getAssignee() == nullptr);
}

// ================================================================== DefinitionBuilder: custom items

TEST_CASE("addCustomItem() places the caller's item in the current stage") {
	DefinitionBuilder b;
	b.reset("W", 1);
	b.addTask("a", "r");                                 // item-0
	b.beginStage("S", false);
	std::shared_ptr<BasicWorkItem> custom = makeItem("custom");
	b.addCustomItem(custom);
	b.endStage();
	b.addTask("b", "r");                                 // numbering carries on: item-1
	std::unique_ptr<WorkflowDefinition> def = finish(b);

	CHECK(visitOrder(def->getRoot().get()) == "item-0 stage-1 custom item-1");
	CHECK(def->getRoot()->find("custom") == custom.get());
	CHECK(def->getTasks().size() == 2);                  // a custom item is not a builder-made task
}

TEST_CASE("addCustomItem() rejects null and adds nothing") {
	DefinitionBuilder b;
	b.reset("W", 1);
	CHECK_THROWS_AS(b.addCustomItem(std::shared_ptr<WorkItem>()), const char*);
	std::unique_ptr<WorkflowDefinition> def = finish(b);
	CHECK(def->getRoot()->childCount() == 0);
}

TEST_CASE("a custom item can be a decorated one") {
	DefinitionBuilder b;
	b.reset("W", 1);
	b.addCustomItem(std::shared_ptr<WorkItem>(new PriorityItem(new BasicWorkItem("urgent", spec("Urgent", "r")), 9)));
	std::unique_ptr<WorkflowDefinition> def = finish(b);
	WorkItem* found = def->getRoot()->find("urgent");
	REQUIRE(found != nullptr);
	CHECK(dynamic_cast<PriorityItem*>(found) != nullptr);
}

TEST_CASE("the definition keeps a custom item alive") {
	std::weak_ptr<WorkflowNode> watch;
	std::unique_ptr<WorkflowDefinition> def;
	{
		DefinitionBuilder b;
		b.reset("W", 1);
		std::shared_ptr<BasicWorkItem> custom = makeItem("kept");
		watch = custom;
		b.addCustomItem(custom);
		def = finish(b);
	}                                                    // caller's pointer and builder are gone
	CHECK_FALSE(watch.expired());
	def.reset();
	CHECK(watch.expired());
}

// ================================================================== DefinitionBuilder: approvals

TEST_CASE("requireApproval() attaches the chain to the task just added") {
	AutoApprovalHandler chain(5);
	DefinitionBuilder b;
	b.reset("W", 1);
	b.addTask("plain", "r");
	b.addTask("gated", "r");
	b.requireApproval("complete", &chain);
	b.addTask("after", "r");
	std::unique_ptr<WorkflowDefinition> def = finish(b);
	const auto& tasks = def->getTasks();

	CHECK_FALSE(tasks[0]->requiresApproval("complete"));
	CHECK(tasks[1]->requiresApproval("complete"));
	CHECK(tasks[1]->getApprovalChain("complete") == &chain);
	CHECK_FALSE(tasks[2]->requiresApproval("complete"));  // only the task it directly followed
	CHECK(tasks[1]->getApprovalChain("start") == nullptr);
	REQUIRE(def->getChains().size() == 1);
	CHECK(def->getChains()[0] == &chain);
}

TEST_CASE("the definition lists each chain once, however many tasks use it") {
	AutoApprovalHandler c1(1), c2(2);
	DefinitionBuilder b;
	b.reset("W", 1);
	b.addTask("a", "r"); b.requireApproval("complete", &c1);
	b.addTask("b", "r"); b.requireApproval("complete", &c1);   // same chain again
	b.addTask("c", "r"); b.requireApproval("complete", &c2);
	std::unique_ptr<WorkflowDefinition> def = finish(b);

	REQUIRE(def->getChains().size() == 2);
	CHECK(def->getChains()[0] == &c1);
	CHECK(def->getChains()[1] == &c2);
	CHECK(def->getTasks()[0]->getApprovalChain("complete") == &c1);
	CHECK(def->getTasks()[1]->getApprovalChain("complete") == &c1);
	CHECK(def->getTasks()[2]->getApprovalChain("complete") == &c2);
}

TEST_CASE("one task can carry approvals for several triggers") {
	AutoApprovalHandler c1(1), c2(2);
	DefinitionBuilder b;
	b.reset("W", 1);
	b.addTask("a", "r");
	b.requireApproval("start", &c1);
	b.requireApproval("complete", &c2);
	std::unique_ptr<WorkflowDefinition> def = finish(b);
	CHECK(def->getTasks()[0]->getApprovalChain("start") == &c1);
	CHECK(def->getTasks()[0]->getApprovalChain("complete") == &c2);
	CHECK(def->getChains().size() == 2);
}

TEST_CASE("requireApproval() must directly follow addTask()") {
	AutoApprovalHandler chain(5);

	SUBCASE("at the very start") {
		DefinitionBuilder b; b.reset("W", 1);
		CHECK_THROWS_AS(b.requireApproval("complete", &chain), const char*);
	}
	SUBCASE("after beginStage") {
		DefinitionBuilder b; b.reset("W", 1);
		b.addTask("a", "r");
		b.beginStage("S", false);
		CHECK_THROWS_AS(b.requireApproval("complete", &chain), const char*);
	}
	SUBCASE("after endStage") {
		DefinitionBuilder b; b.reset("W", 1);
		b.beginStage("S", false);
		b.addTask("a", "r");
		b.endStage();
		CHECK_THROWS_AS(b.requireApproval("complete", &chain), const char*);
	}
	SUBCASE("after a custom item") {
		DefinitionBuilder b; b.reset("W", 1);
		b.addTask("a", "r");
		b.addCustomItem(makeItem("c"));
		CHECK_THROWS_AS(b.requireApproval("complete", &chain), const char*);
	}
}

TEST_CASE("a refused requireApproval() changes nothing") {
	AutoApprovalHandler chain(5);
	DefinitionBuilder b;
	b.reset("W", 1);
	b.addTask("a", "r");
	CHECK_THROWS_AS(b.requireApproval("complete", nullptr), const char*);
	std::unique_ptr<WorkflowDefinition> def = finish(b);
	CHECK_FALSE(def->getTasks()[0]->requiresApproval("complete"));
	CHECK(def->getChains().empty());
}

// ================================================================== WorkflowDefinition (the product)

TEST_CASE("a new definition is empty") {
	WorkflowDefinition def("Empty", 3);
	CHECK(def.getName() == "Empty");
	CHECK(def.getVersion() == 3);
	CHECK(def.getRoot() == nullptr);
	CHECK(def.getTasks().empty());
	CHECK(def.getChains().empty());
}

TEST_CASE("a definition stores its root, tasks and chains") {
	WorkflowDefinition def("D", 1);
	std::shared_ptr<WorkflowNode> root = makeStage("r");
	def.setRoot(root);
	CHECK(def.getRoot() == root);
	def.setRoot(nullptr);
	CHECK(def.getRoot() == nullptr);

	std::shared_ptr<Task> t1 = spec("one", "r"), t2 = spec("two", "r");
	def.addTask(t1); def.addTask(t2);
	REQUIRE(def.getTasks().size() == 2);
	CHECK(def.getTasks()[0] == t1);
	CHECK(def.getTasks()[1] == t2);

	AutoApprovalHandler c1(1), c2(2);
	def.addChain(&c1); def.addChain(&c2); def.addChain(&c1);
	REQUIRE(def.getChains().size() == 2);
	CHECK(def.getChains()[0] == &c1);
	CHECK(def.getChains()[1] == &c2);
}

TEST_CASE("a definition rejects null tasks and chains") {
	WorkflowDefinition def("D", 1);
	CHECK_THROWS_AS(def.addTask(nullptr), const char*);
	CHECK_THROWS_AS(def.addChain(nullptr), const char*);
	CHECK(def.getTasks().empty());
	CHECK(def.getChains().empty());
}

// ================================================================== Builder products at run time

TEST_CASE("running a definition never changes the definition") {
	DefinitionBuilder b;
	b.reset("W", 1);
	b.addTask("a", "r");
	b.addTask("b", "r");
	std::unique_ptr<WorkflowDefinition> def = finish(b);
	Participant ann("Ann", 1, roles("r"));

	std::unique_ptr<WorkflowInstance> run1(def->createInstance("run-1"));
	std::unique_ptr<WorkflowInstance> run2(def->createInstance("run-2"));
	REQUIRE(run1->getRoot() != nullptr);
	CHECK(run1->getRoot() != def->getRoot().get());
	CHECK(run1->getRoot() != run2->getRoot());

	work(*run1->getRoot()->find("item-0"), ann);
	CHECK(run1->getRoot()->find("item-0")->isComplete());
	CHECK(run2->getRoot()->find("item-0")->getStateName() == "Pending");
	CHECK(def->getRoot()->find("item-0")->getStateName() == "Pending");
	CHECK(def->getRoot()->find("item-0")->getAssignee() == nullptr);
}

TEST_CASE("an approval declared in the builder gates the running item") {
	AutoApprovalHandler chain(5);
	DefinitionBuilder b;
	b.reset("W", 1);
	b.addTask("Discharge", "doctor");
	b.requireApproval("complete", &chain);
	std::unique_ptr<WorkflowDefinition> def = finish(b);
	Participant doc("Dee", 1, roles("doctor"));

	std::unique_ptr<WorkflowInstance> run(def->createInstance("run"));
	WorkItem* item = run->getRoot()->find("item-0");
	item->activate(); item->assign(&doc); item->start(); item->complete();
	CHECK(item->getStateName() == "AwaitingApproval");
	CHECK_FALSE(run->getRoot()->isComplete());
	item->approve();
	CHECK(item->getStateName() == "Completed");
	CHECK(run->getRoot()->isComplete());
}

// ================================================================== Director -> builder call sequence

TEST_CASE("the Director turns a script into exactly the right builder calls") {
	RecordingBuilder rb;
	REQUIRE(runScript(admissionScript(), rb));
	std::vector<std::string> expected = {
		"addTask Triage | nurse",
		"beginStage Tests parallel",
		"addTask Blood test | nurse",
		"addTask X-ray | radiographer",
		"endStage",
		"addTask Discharge | doctor"};
	CHECK(rb.calls == expected);                          // and it never calls reset() or getResult() itself
}

TEST_CASE("the Director ignores blank lines, comments and surrounding whitespace") {
	RecordingBuilder rb;
	std::string script =
		"\n"
		"   \n"
		"# a comment\n"
		"    # an indented comment | with | pipes\n"
		"  task  |  Triage  |  nurse  \n"
		"\ttask\t|\tX-ray\t|\tradiographer\t\n"
		"\n";
	REQUIRE(runScript(script, rb));
	std::vector<std::string> expected = {"addTask Triage | nurse", "addTask X-ray | radiographer"};
	CHECK(rb.calls == expected);
}

TEST_CASE("Windows line endings are handled") {
	RecordingBuilder rb;
	REQUIRE(runScript("task | A | nurse\r\nstage | S | parallel\r\ntask | B | doctor\r\nend\r\n", rb));
	std::vector<std::string> expected = {"addTask A | nurse", "beginStage S parallel", "addTask B | doctor", "endStage"};
	CHECK(rb.calls == expected);
}

TEST_CASE("command names are case-insensitive but names and roles keep their case") {
	RecordingBuilder rb;
	REQUIRE(runScript("TASK | Mixed Case | NurseRole\nStage | Big Stage\nEND\n", rb));
	std::vector<std::string> expected = {"addTask Mixed Case | NurseRole", "beginStage Big Stage sequential", "endStage"};
	CHECK(rb.calls == expected);
}

TEST_CASE("a '#' only starts a comment at the beginning of a line") {
	RecordingBuilder rb;
	REQUIRE(runScript("task | Fix bug #12 | developer\n", rb));
	REQUIRE(rb.calls.size() == 1);
	CHECK(rb.calls[0] == "addTask Fix bug #12 | developer");
}

TEST_CASE("stages are sequential unless the script says parallel") {
	RecordingBuilder rb;
	REQUIRE(runScript("stage | A\nstage | B | sequential\nstage | C | parallel\nstage | D | parallel | extra\n", rb));
	std::vector<std::string> expected = {
		"beginStage A sequential", "beginStage B sequential", "beginStage C parallel", "beginStage D parallel"};
	CHECK(rb.calls == expected);
}

TEST_CASE("an empty script, or one with only comments, is a valid script that builds nothing") {
	RecordingBuilder rb;
	CHECK(runScript("", rb));
	CHECK(runScript("\n\n# nothing to see\n   \n", rb));
	CHECK(rb.calls.empty());
}

TEST_CASE("the Director stops at the first bad line and reports failure") {
	CerrCapture quiet;

	SUBCASE("an unknown command") {
		RecordingBuilder rb;
		CHECK_FALSE(runScript("task | A | r\nfly | to | moon\ntask | B | r\n", rb));
		REQUIRE(rb.calls.size() == 1);                   // the line before ran; the one after never did
		CHECK(rb.calls[0] == "addTask A | r");
	}
	SUBCASE("a task with no role") {
		RecordingBuilder rb;
		CHECK_FALSE(runScript("task | OnlyAName\ntask | B | r\n", rb));
		CHECK(rb.calls.empty());
	}
	SUBCASE("a stage with no name") {
		RecordingBuilder rb;
		CHECK_FALSE(runScript("stage\ntask | B | r\n", rb));
		CHECK(rb.calls.empty());
	}
	SUBCASE("a bare task command") {
		RecordingBuilder rb;
		CHECK_FALSE(runScript("task\n", rb));
		CHECK(rb.calls.empty());
	}
}

TEST_CASE("the error message names the line that went wrong") {
	CerrCapture err;
	RecordingBuilder rb;
	CHECK_FALSE(runScript("# header\n\ntask | A | r\nfly | away\n", rb));   // line 4
	CHECK(contains(err.str(), "4"));
	CHECK_FALSE(contains(err.str(), "3"));
}

TEST_CASE("builder errors are caught by the Director and reported with a line number") {
	CerrCapture err;
	DefinitionBuilder b;
	b.reset("W", 1);
	CHECK_FALSE(runScript("task | A | r\nend\n", b));   // line 2: endStage() with no open stage
	CHECK(contains(err.str(), "2"));
	CHECK_NOTHROW(b.addTask("builder still usable", "r"));
}

TEST_CASE("a Director can only read its script once") {
	std::istringstream in("task | A | r\n");
	WorkflowDirector director(in);
	RecordingBuilder first, second;
	CHECK(director.construct(first));
	CHECK(first.calls.size() == 1);
	CHECK(director.construct(second));                   // the stream is used up
	CHECK(second.calls.empty());
}

TEST_CASE("registering approval chains is harmless") {
	std::istringstream in("");
	WorkflowDirector director(in);
	AutoApprovalHandler chain(5);
	CHECK_NOTHROW(director.registerApprovalChain("manager", &chain));
	CHECK_NOTHROW(director.registerApprovalChain("manager", &chain));     // same name again
	CHECK_NOTHROW(director.registerApprovalChain("ignored", nullptr));    // null is ignored, not stored
}

// ================================================================== Director + DefinitionBuilder together

TEST_CASE("a script and hand-written builder calls produce the same tree") {
	std::unique_ptr<WorkflowDefinition> viaScript(buildDefinition(admissionScript()));
	REQUIRE(viaScript != nullptr);

	DefinitionBuilder b;
	b.reset("Admission", 1);
	b.addTask("Triage", "nurse");
	b.beginStage("Tests", true);
	b.addTask("Blood test", "nurse");
	b.addTask("X-ray", "radiographer");
	b.endStage();
	b.addTask("Discharge", "doctor");
	std::unique_ptr<WorkflowDefinition> byHand = finish(b);

	CHECK(visitOrder(viaScript->getRoot().get()) == visitOrder(byHand->getRoot().get()));
	REQUIRE(viaScript->getTasks().size() == byHand->getTasks().size());
	for (size_t i = 0; i < byHand->getTasks().size(); ++i) {
		CHECK(viaScript->getTasks()[i]->getName() == byHand->getTasks()[i]->getName());
		CHECK(viaScript->getTasks()[i]->getRequiredRole() == byHand->getTasks()[i]->getRequiredRole());
	}
}

TEST_CASE("scripts of different shapes build the matching trees") {
	SUBCASE("flat") {
		std::unique_ptr<WorkflowDefinition> d(buildDefinition("task | a | r\ntask | b | r\ntask | c | r\n"));
		REQUIRE(d != nullptr);
		CHECK(visitOrder(d->getRoot().get()) == "item-0 item-1 item-2");
	}
	SUBCASE("three levels deep") {
		std::unique_ptr<WorkflowDefinition> d(buildDefinition(
			"stage | one\n stage | two | parallel\n  stage | three\n   task | deep | r\n  end\n end\nend\ntask | top | r\n"));
		REQUIRE(d != nullptr);
		CHECK(visitOrder(d->getRoot().get()) == "stage-1 stage-2 stage-3 item-0 item-1");
	}
	SUBCASE("back-to-back stages") {
		std::unique_ptr<WorkflowDefinition> d(buildDefinition(
			"stage | a\ntask | x | r\nend\nstage | b | parallel\ntask | y | r\nend\n"));
		REQUIRE(d != nullptr);
		CHECK(d->getRoot()->childCount() == 2);
		CHECK(dynamic_cast<Stage*>(d->getRoot()->child(1))->isParallel());
	}
	SUBCASE("an unclosed stage parses, but the builder refuses to finish it") {
		CerrCapture quiet;
		DefinitionBuilder b; b.reset("W", 1);
		CHECK(runScript("stage | open\ntask | a | r\n", b));
		CHECK_THROWS_AS(b.getResult(), const char*);
	}
}

TEST_CASE("a scripted workflow can be run from start to finish") {
	std::unique_ptr<WorkflowDefinition> def(buildDefinition(admissionScript()));
	REQUIRE(def != nullptr);
	std::unique_ptr<WorkflowInstance> run(def->createInstance("admission-1"));
	WorkflowNode* root = run->getRoot();
	Participant p("Pat", 1, roles("any"));

	root->activate();
	CHECK(root->find("item-0")->isActive());             // Triage
	CHECK_FALSE(root->find("item-1")->isActive());       // the parallel tests wait for it
	work(*root->find("item-0"), p);

	root->activate();
	CHECK(root->find("item-1")->isActive());             // Blood test and X-ray open together
	CHECK(root->find("item-2")->isActive());
	CHECK_FALSE(root->find("item-3")->isActive());
	work(*root->find("item-1"), p);
	root->activate();
	CHECK_FALSE(root->find("item-3")->isActive());       // X-ray still outstanding
	work(*root->find("item-2"), p);

	root->activate();
	CHECK(root->find("item-3")->isActive());             // Discharge
	CHECK_FALSE(root->isComplete());
	work(*root->find("item-3"), p);
	CHECK(root->isComplete());
	CHECK(run->isComplete());
}

}
