// Thorough Composite tests: structure, ownership, completion, activation, binding, cloning, lookup, iteration.
#include <algorithm>
#include "doctest.h"
#include "test_tree_helpers.h"

using namespace testutil;

TEST_SUITE("Composite (thorough)") {

// ---------------------------------------------------------------- structure

TEST_CASE("an empty composite is inert and never counts as complete") {
	auto stage = makeStage("s");
	CHECK(stage->childCount() == 0);
	CHECK_FALSE(stage->isComplete());         // an empty node must not release a join early
	CHECK_NOTHROW(stage->activate());
	CHECK(visitOrder(stage.get()) == "");
	CHECK(stage->find("anything") == nullptr);
	CHECK(stage->asWorkItem() == nullptr);
	std::unique_ptr<WorkflowNode> copy(stage->clone());
	CHECK(copy->childCount() == 0);
}

TEST_CASE("children keep insertion order and child(i) checks its bounds") {
	auto stage = makeStage("s");
	auto a = makeItem("a"), b = makeItem("b"), c = makeItem("c");
	stage->add(a); stage->add(b); stage->add(c);

	CHECK(stage->childCount() == 3);
	CHECK(stage->child(0) == a.get());
	CHECK(stage->child(1) == b.get());
	CHECK(stage->child(2) == c.get());
	CHECK(visitOrder(stage.get()) == "a b c");
	CHECK_THROWS_AS(stage->child(3), std::out_of_range);
	CHECK_THROWS_AS(stage->child(-1), std::out_of_range);
	CHECK_THROWS_AS(stage->child(1000), std::out_of_range);
}

TEST_CASE("a leaf has no children and knows it is a work item") {
	auto leaf = makeItem("leaf");
	CHECK(leaf->childCount() == 0);
	CHECK_THROWS_AS(leaf->child(0), std::out_of_range);
	CHECK(leaf->asWorkItem() == leaf.get());
	CHECK(visitOrder(leaf.get()) == "");      // NullIterator
	CHECK(leaf->getId() == "leaf");
	CHECK(leaf->getName() == "Task leaf");
	CHECK(leaf->find("leaf") == leaf.get());
	CHECK(leaf->find("other") == nullptr);
}

TEST_CASE("composites report their id, name and parallel flag") {
	auto seq = makeStage("seq", false);
	auto par = makeStage("par", true);
	CHECK(seq->getId() == "seq");
	CHECK(seq->getName() == "Stage seq");
	CHECK_FALSE(seq->isParallel());
	CHECK(par->isParallel());
	auto sub = makeSub("sub", 4);
	CHECK(sub->getSourceVersion() == 4);
}

// ---------------------------------------------------------------- add()

TEST_CASE("add() rejects bad nodes and leaves the tree exactly as it was") {
	auto root = makeStage("root");
	auto mid = makeStage("mid");
	auto leaf = makeItem("leaf");
	root->add(mid);
	mid->add(leaf);

	CHECK_THROWS_AS(root->add(std::shared_ptr<WorkflowNode>()), std::invalid_argument);   // null
	CHECK_THROWS_AS(root->add(mid), std::invalid_argument);        // same child twice
	CHECK_THROWS_AS(root->add(leaf), std::invalid_argument);       // already deeper in this tree
	CHECK_THROWS_AS(mid->add(mid), std::invalid_argument);         // itself
	CHECK_THROWS_AS(mid->add(root), std::invalid_argument);        // would make a cycle

	CHECK(root->childCount() == 1);
	CHECK(mid->childCount() == 1);
	CHECK(visitOrder(root.get()) == "mid leaf");
}

TEST_CASE("a longer cycle is refused too") {
	auto a = makeStage("a"), b = makeStage("b"), c = makeStage("c");
	a->add(b);
	b->add(c);
	CHECK_THROWS_AS(c->add(a), std::invalid_argument);             // a -> b -> c -> a
	CHECK(c->childCount() == 0);
}

TEST_CASE("add() shares ownership with the caller") {
	auto leaf = makeItem("leaf");
	std::weak_ptr<WorkflowNode> watch = leaf;
	{
		auto stage = makeStage("s");
		stage->add(leaf);
		CHECK(leaf.use_count() == 2);      // caller + stage
	}
	CHECK(leaf.use_count() == 1);          // the stage is gone, the caller's node lives on
	CHECK_FALSE(watch.expired());
	leaf.reset();
	CHECK(watch.expired());
}

TEST_CASE("add() does not require the caller to keep a pointer") {
	auto stage = makeStage("s");
	stage->add(makeItem("temp"));          // only the stage owns it
	REQUIRE(stage->childCount() == 1);
	CHECK(stage->child(0)->getId() == "temp");
	CHECK(stage->child(0)->asWorkItem() != nullptr);
}

// ---------------------------------------------------------------- remove()

TEST_CASE("remove() detaches a direct child and keeps the others in order") {
	auto stage = makeStage("s");
	auto a = makeItem("a"), b = makeItem("b"), c = makeItem("c");
	stage->add(a); stage->add(b); stage->add(c);

	stage->remove(b.get());
	CHECK(stage->childCount() == 2);
	CHECK(visitOrder(stage.get()) == "a c");
	CHECK(b.use_count() == 1);             // only the caller still holds it
	CHECK(b->getId() == "b");              // and it is still perfectly usable

	stage->remove(a.get());
	stage->remove(c.get());
	CHECK(stage->childCount() == 0);
	CHECK_FALSE(stage->isComplete());
}

TEST_CASE("remove() only accepts direct children") {
	auto root = makeStage("root");
	auto mid = makeStage("mid");
	auto deep = makeItem("deep");
	auto stranger = makeItem("stranger");
	root->add(mid);
	mid->add(deep);

	CHECK_THROWS_AS(root->remove(deep.get()), std::invalid_argument);       // grandchild
	CHECK_THROWS_AS(root->remove(stranger.get()), std::invalid_argument);   // never added
	CHECK_THROWS_AS(root->remove(nullptr), std::invalid_argument);
	mid->remove(deep.get());
	CHECK_THROWS_AS(mid->remove(deep.get()), std::invalid_argument);       // already gone
	CHECK(visitOrder(root.get()) == "mid");
}

TEST_CASE("a removed node is destroyed when nobody else holds it") {
	int destroyed = 0;
	auto stage = makeStage("s");
	stage->add(std::make_shared<SpyItem>("spy", spec("Spy", "nurse"), &destroyed));
	REQUIRE(stage->childCount() == 1);
	stage->remove(stage->child(0));
	CHECK(destroyed == 1);
	CHECK(stage->childCount() == 0);
}

// ---------------------------------------------------------------- completion

TEST_CASE("a composite completes exactly when its last child completes") {
	Participant ann("Ann", 1, roles("nurse"));
	auto stage = makeStage("s", true);
	auto a = makeItem("a"), b = makeItem("b"), c = makeItem("c");
	stage->add(a); stage->add(b); stage->add(c);

	CHECK_FALSE(stage->isComplete());
	work(*a, ann);  CHECK_FALSE(stage->isComplete());
	work(*c, ann);  CHECK_FALSE(stage->isComplete());   // order of completion does not matter
	work(*b, ann);  CHECK(stage->isComplete());
}

TEST_CASE("a rejected or escalated child keeps its stage incomplete") {
	Participant ann("Ann", 1, roles("nurse"));
	auto stage = makeStage("s", true);
	auto ok = makeItem("ok"), bad = makeItem("bad"), stuck = makeItem("stuck");
	stage->add(ok); stage->add(bad); stage->add(stuck);

	work(*ok, ann);
	bad->activate(); bad->assign(&ann); bad->start(); bad->reject("wrong");
	stuck->activate(); stuck->assign(&ann); stuck->escalate();
	CHECK(bad->getStateName() == "Rejected");
	CHECK(stuck->getStateName() == "Escalated");
	CHECK_FALSE(stage->isComplete());
}

TEST_CASE("nesting: every level has to be complete") {
	Participant ann("Ann", 1, roles("nurse"));
	auto root = makeStage("root");
	auto inner = makeSub("inner");
	auto top = makeItem("top"), deep = makeItem("deep");
	root->add(top);
	root->add(inner);
	inner->add(deep);

	work(*top, ann);
	CHECK_FALSE(inner->isComplete());
	CHECK_FALSE(root->isComplete());       // the outer node waits for the inner one
	work(*deep, ann);
	CHECK(inner->isComplete());
	CHECK(root->isComplete());
}

TEST_CASE("completion follows later changes to the tree") {
	Participant ann("Ann", 1, roles("nurse"));
	auto stage = makeStage("s", true);
	auto a = makeItem("a");
	stage->add(a);
	work(*a, ann);
	CHECK(stage->isComplete());

	auto late = makeItem("late");
	stage->add(late);                      // new unfinished work re-opens the stage
	CHECK_FALSE(stage->isComplete());
	stage->remove(late.get());             // removing it closes the stage again
	CHECK(stage->isComplete());
}

// ---------------------------------------------------------------- activation

static std::string states(const std::vector<std::shared_ptr<BasicWorkItem>>& items) {
	std::string out;
	for (size_t i = 0; i < items.size(); ++i) out += (i ? " " : "") + items[i]->getStateName();
	return out;
}

TEST_CASE("a sequential stage opens one child at a time, in order") {
	Participant ann("Ann", 1, roles("nurse"));
	auto stage = makeStage("s", false);
	std::vector<std::shared_ptr<BasicWorkItem>> v;
	for (std::string id : {"a", "b", "c"}) { v.push_back(makeItem(id)); stage->add(v.back()); }

	stage->activate();
	CHECK(states(v) == "Available Pending Pending");
	stage->activate();                                   // nothing finished yet: nothing new opens
	CHECK(states(v) == "Available Pending Pending");

	work(*v[0], ann);
	stage->activate();
	CHECK(states(v) == "Completed Available Pending");
	work(*v[1], ann);
	stage->activate();
	CHECK(states(v) == "Completed Completed Available");
	work(*v[2], ann);
	CHECK(stage->isComplete());
}

TEST_CASE("a sequential stage skips children that are already complete") {
	Participant ann("Ann", 1, roles("nurse"));
	auto stage = makeStage("s", false);
	std::vector<std::shared_ptr<BasicWorkItem>> v;
	for (std::string id : {"a", "b", "c"}) { v.push_back(makeItem(id)); stage->add(v.back()); }

	work(*v[1], ann);                                    // b was done out of order
	stage->activate();
	CHECK(states(v) == "Available Completed Pending");
	work(*v[0], ann);
	stage->activate();
	CHECK(states(v) == "Completed Completed Available");
}

TEST_CASE("a parallel stage opens every unfinished child at once") {
	Participant ann("Ann", 1, roles("nurse"));
	auto stage = makeStage("s", true);
	std::vector<std::shared_ptr<BasicWorkItem>> v;
	for (std::string id : {"a", "b", "c", "d"}) { v.push_back(makeItem(id)); stage->add(v.back()); }

	work(*v[2], ann);                                    // already finished: must not be touched
	stage->activate();
	CHECK(states(v) == "Available Available Completed Available");
}

TEST_CASE("activating twice is harmless: no extra transitions, no extra events") {
	auto stage = makeStage("s", true);
	auto a = makeItem("a"), b = makeItem("b");
	stage->add(a); stage->add(b);
	WorkflowSubject subject; Recorder rec; subject.attach(&rec);
	stage->bind(&subject);

	stage->activate();
	CHECK(rec.count("state.changed") == 2);
	stage->activate();
	stage->activate();
	CHECK(rec.count("state.changed") == 2);
	CHECK(a->getStateName() == "Available");
}

TEST_CASE("activating a finished stage changes nothing") {
	Participant ann("Ann", 1, roles("nurse"));
	auto stage = makeStage("s", true);
	auto a = makeItem("a");
	stage->add(a);
	work(*a, ann);
	WorkflowSubject subject; Recorder rec; subject.attach(&rec);
	stage->bind(&subject);
	stage->activate();
	CHECK(rec.types.empty());
	CHECK(a->getStateName() == "Completed");
}

TEST_CASE("sequential inside parallel, and parallel inside sequential") {
	Participant ann("Ann", 1, roles("nurse"));

	SUBCASE("sequential lanes inside a parallel stage run side by side") {
		auto fork = makeStage("fork", true);
		auto laneA = makeStage("laneA", false), laneB = makeStage("laneB", false);
		auto a1 = makeItem("a1"), a2 = makeItem("a2"), b1 = makeItem("b1"), b2 = makeItem("b2");
		laneA->add(a1); laneA->add(a2); laneB->add(b1); laneB->add(b2);
		fork->add(laneA); fork->add(laneB);

		fork->activate();
		CHECK(a1->isActive()); CHECK_FALSE(a2->isActive());
		CHECK(b1->isActive()); CHECK_FALSE(b2->isActive());
		work(*a1, ann);
		fork->activate();
		CHECK(a2->isActive());
		CHECK_FALSE(b2->isActive());                     // lane B is still on its first task
	}

	SUBCASE("a parallel stage inside a sequence is a gate") {
		auto seq = makeStage("seq", false);
		auto fork = makeStage("fork", true);
		auto x = makeItem("x"), y = makeItem("y"), after = makeItem("after");
		fork->add(x); fork->add(y);
		seq->add(fork); seq->add(after);

		seq->activate();
		CHECK(x->isActive()); CHECK(y->isActive());
		CHECK_FALSE(after->isActive());
		work(*x, ann);
		seq->activate();
		CHECK_FALSE(after->isActive());                  // y still outstanding
		work(*y, ann);
		seq->activate();
		CHECK(after->isActive());                        // join released
	}
}

TEST_CASE("SubProcess opens all of its unfinished children and gates the parent") {
	Participant ann("Ann", 1, roles("nurse"));
	auto root = makeStage("root", false);
	auto sub = makeSub("sub", 3);
	auto s1 = makeItem("s1"), s2 = makeItem("s2"), next = makeItem("next");
	sub->add(s1); sub->add(s2);
	root->add(sub); root->add(next);

	root->activate();
	CHECK(s1->isActive()); CHECK(s2->isActive());        // sub-process children run together
	CHECK_FALSE(next->isActive());                       // the parent is waiting
	work(*s1, ann);
	root->activate();
	CHECK_FALSE(next->isActive());
	work(*s2, ann);
	root->activate();
	CHECK(next->isActive());
}

// ---------------------------------------------------------------- bind() / events

TEST_CASE("bind() reaches children added before AND after, at any depth") {
	auto root = makeStage("root", true);
	auto mid = makeStage("mid", true);
	auto early = makeItem("early"), deepEarly = makeItem("deepEarly");
	mid->add(deepEarly);
	root->add(early);
	root->add(mid);

	WorkflowSubject subject; Recorder rec; subject.attach(&rec);
	root->bind(&subject);

	auto late = makeItem("late"), deepLate = makeItem("deepLate");
	root->add(late);                                     // added after binding
	mid->add(deepLate);                                  // added to an already-bound nested stage

	root->activate();
	CHECK(rec.count("state.changed") == 4);
	std::vector<std::string> got = rec.itemIds;
	std::sort(got.begin(), got.end());
	std::vector<std::string> want = {"deepEarly", "deepLate", "early", "late"};
	CHECK(got == want);
}

TEST_CASE("events carry the new state name") {
	Participant ann("Ann", 1, roles("nurse"));
	auto stage = makeStage("s");
	auto a = makeItem("a");
	stage->add(a);
	WorkflowSubject subject; Recorder rec; subject.attach(&rec);
	stage->bind(&subject);

	work(*a, ann);
	std::vector<std::string> want = {"Available", "Assigned", "InProgress", "Completed"};
	CHECK(rec.details == want);
	CHECK(rec.count("state.changed") == 4);
}

TEST_CASE("an unbound tree is silent; rebinding moves the events to the new subject") {
	Participant ann("Ann", 1, roles("nurse"));
	auto stage = makeStage("s", true);
	auto a = makeItem("a"), b = makeItem("b");
	stage->add(a); stage->add(b);

	WorkflowSubject first, second; Recorder r1, r2;
	first.attach(&r1); second.attach(&r2);

	stage->activate();                                   // never bound
	CHECK(r1.types.empty());

	stage->bind(&first);
	a->assign(&ann);
	CHECK(r1.count("state.changed") == 1);

	stage->bind(&second);
	b->assign(&ann);
	CHECK(r1.count("state.changed") == 1);               // nothing more for the old subject
	CHECK(r2.count("state.changed") == 1);

	stage->bind(nullptr);
	a->start();
	CHECK(r1.count("state.changed") == 1);
	CHECK(r2.count("state.changed") == 1);

	stage->add(makeItem("c"));                           // a child added to an unbound stage stays unbound
	stage->child(2)->activate();
	CHECK(r2.count("state.changed") == 1);
}

// ---------------------------------------------------------------- clone()

namespace {
std::shared_ptr<Stage> buildStudioTree(std::shared_ptr<BasicWorkItem>* outFirst = nullptr) {
	auto root = makeStage("root", false);
	auto content = makeStage("content", true);
	auto art = makeSub("art", 2);
	auto first = makeItem("model");
	art->add(first);
	art->add(makeItem("texture"));
	content->add(art);
	content->add(makeItem("code"));
	root->add(content);
	root->add(makeItem("release"));
	if (outFirst) *outFirst = first;
	return root;
}
}

TEST_CASE("clone() is a deep copy: same shape, different objects") {
	auto root = buildStudioTree();
	std::unique_ptr<WorkflowNode> copy(root->clone());

	CHECK(copy.get() != root.get());
	CHECK(copy->getId() == "root");
	CHECK(copy->getName() == root->getName());
	CHECK(visitOrder(copy.get()) == visitOrder(root.get()));
	CHECK(visitOrder(copy.get()) == "content art model texture code release");

	// no node is shared between the two trees
	std::unique_ptr<WorkflowIterator> a(root->createIterator()), b(copy->createIterator());
	for (a->first(), b->first(); !a->isDone() && !b->isDone(); a->next(), b->next())
		CHECK(a->current() != b->current());

	// but the read-only Task spec is shared
	CHECK(&root->find("model")->getTask() == &copy->find("model")->getTask());
}

TEST_CASE("clone() keeps the stage flags and the sub-process version") {
	auto root = buildStudioTree();
	std::unique_ptr<WorkflowNode> copy(root->clone());

	Stage* content = dynamic_cast<Stage*>(copy->child(0));
	REQUIRE(content != nullptr);
	CHECK(content->isParallel());
	CHECK_FALSE(dynamic_cast<Stage*>(copy.get())->isParallel());
	SubProcess* art = dynamic_cast<SubProcess*>(content->child(0));
	REQUIRE(art != nullptr);
	CHECK(art->getSourceVersion() == 2);

	// the parallel flag really changes behaviour in the copy
	content->activate();
	CHECK(copy->find("model")->isActive());
	CHECK(copy->find("code")->isActive());
}

TEST_CASE("the original and its clone evolve independently") {
	Participant ann("Ann", 1, roles("nurse"));
	auto root = buildStudioTree();
	std::unique_ptr<WorkflowNode> copy(root->clone());

	SUBCASE("working on the clone leaves the original alone") {
		work(*copy->find("model"), ann);
		CHECK(copy->find("model")->isComplete());
		CHECK(root->find("model")->getStateName() == "Pending");
		CHECK(root->find("model")->getAssignee() == nullptr);
	}
	SUBCASE("working on the original leaves the clone alone") {
		work(*root->find("code"), ann);
		CHECK(root->find("code")->isComplete());
		CHECK(copy->find("code")->getStateName() == "Pending");
	}
	SUBCASE("structural changes are independent too") {
		auto extra = makeItem("extra");
		static_cast<CompositeNode*>(copy.get())->add(extra);
		CHECK(copy->childCount() == 3);
		CHECK(root->childCount() == 2);
	}
}

TEST_CASE("cloning mid-run copies each item's progress") {
	Participant ann("Ann", 1, roles("nurse"));
	auto root = buildStudioTree();
	root->activate();
	work(*root->find("model"), ann);
	root->find("texture")->assign(&ann);

	std::unique_ptr<WorkflowNode> copy(root->clone());
	CHECK(copy->find("model")->isComplete());
	CHECK(copy->find("texture")->getStateName() == "Assigned");
	CHECK(copy->find("texture")->getAssignee() == &ann);
	CHECK(copy->find("code")->getStateName() == "Available");
	CHECK(copy->find("release")->getStateName() == "Pending");

	// the copy carries on from where the original was
	copy->find("texture")->start();
	copy->find("texture")->complete();
	CHECK(copy->child(0)->child(0)->isComplete());      // the Art sub-process finished in the copy
	CHECK_FALSE(root->child(0)->child(0)->isComplete()); // not in the original
}

TEST_CASE("a clone starts unbound: its events never reach the original's observers") {
	Participant ann("Ann", 1, roles("nurse"));
	auto root = buildStudioTree();
	WorkflowSubject subject; Recorder rec; subject.attach(&rec);
	root->bind(&subject);

	std::unique_ptr<WorkflowNode> copy(root->clone());
	copy->activate();
	work(*copy->find("model"), ann);
	CHECK(rec.types.empty());

	root->activate();                                    // the original still reports
	CHECK(rec.count("state.changed") > 0);
}

TEST_CASE("clone() of a single work item") {
	Participant ann("Ann", 1, roles("nurse"));
	auto a = makeItem("a");
	a->setField("note", "hello");
	std::unique_ptr<WorkflowNode> copy(a->clone());
	WorkItem* c = copy->asWorkItem();
	REQUIRE(c != nullptr);
	CHECK(c != a.get());
	CHECK(c->getId() == "a");
	CHECK(c->getField("note") == "hello");
	c->setField("note", "changed");
	CHECK(a->getField("note") == "hello");               // payloads are separate maps
}

// ---------------------------------------------------------------- find()

TEST_CASE("find() looks through every level and only returns work items") {
	auto root = buildStudioTree();
	CHECK(root->find("model") == root->child(0)->child(0)->child(0));
	CHECK(root->find("release") == root->child(1));
	CHECK(root->find("texture") != nullptr);
	CHECK(root->find("nope") == nullptr);
	CHECK(root->find("") == nullptr);
	// stage and sub-process ids are not work items
	CHECK(root->find("content") == nullptr);
	CHECK(root->find("art") == nullptr);
	CHECK(root->find("root") == nullptr);
}

TEST_CASE("find() sees items added later and forgets removed ones") {
	auto root = makeStage("root");
	auto mid = makeStage("mid");
	root->add(mid);
	CHECK(root->find("x") == nullptr);
	auto x = makeItem("x");
	mid->add(x);
	CHECK(root->find("x") == x.get());
	mid->remove(x.get());
	CHECK(root->find("x") == nullptr);
}

// ---------------------------------------------------------------- iteration

TEST_CASE("depth-first order on an unbalanced tree") {
	auto root = makeStage("r");
	auto a = makeStage("a"), b = makeStage("b");
	auto a1 = makeItem("a1");
	auto a2 = makeStage("a2");
	auto a21 = makeItem("a21");
	auto b1 = makeItem("b1");
	a2->add(a21);
	a->add(a1); a->add(a2);
	b->add(b1);
	root->add(a); root->add(b); root->add(makeItem("c"));
	CHECK(visitOrder(root.get()) == "a a1 a2 a21 b b1 c");
}

TEST_CASE("very deep and very wide trees are handled") {
	SUBCASE("100 levels deep") {
		auto root = makeStage("n0");
		std::shared_ptr<Stage> cur = root;
		std::string want;
		for (int i = 1; i < 100; ++i) {
			auto next = makeStage("n" + std::to_string(i));
			cur->add(next);
			want += (want.empty() ? "" : " ") + next->getId();
			cur = next;
		}
		auto leaf = makeItem("leaf");
		cur->add(leaf);
		want += " leaf";
		CHECK(visitOrder(root.get()) == want);
		CHECK_THROWS_AS(cur->add(root), std::invalid_argument);   // the cycle check walks the whole depth
		std::unique_ptr<WorkflowNode> copy(root->clone());
		CHECK(visitOrder(copy.get()) == want);
		CHECK(copy->find("leaf") != nullptr);
	}
	SUBCASE("500 children") {
		auto root = makeStage("wide", true);
		for (int i = 0; i < 500; ++i) root->add(makeItem("i" + std::to_string(i)));
		CHECK(root->childCount() == 500);
		root->activate();
		CHECK(root->child(499)->asWorkItem()->isActive());
		CHECK(root->find("i250") == root->child(250));
		int n = 0;
		std::unique_ptr<WorkflowIterator> it(root->createIterator());
		for (it->first(); !it->isDone(); it->next()) ++n;
		CHECK(n == 500);
	}
}

TEST_CASE("an iterator can be restarted and is null once finished") {
	auto root = makeStage("r");
	root->add(makeItem("a")); root->add(makeItem("b"));
	std::unique_ptr<WorkflowIterator> it(root->createIterator());
	CHECK(it->current()->getId() == "a");                // usable straight after creation
	it->next(); it->next();
	CHECK(it->isDone());
	CHECK(it->current() == nullptr);
	it->next();                                          // stepping past the end is harmless
	CHECK(it->isDone());
	it->first();
	CHECK_FALSE(it->isDone());
	CHECK(it->current()->getId() == "a");
}

// ---------------------------------------------------------------- ownership

TEST_CASE("destroying the root destroys every node, and no caller-held node dies early") {
	int destroyed = 0;
	std::shared_ptr<SpyItem> held;
	{
		auto root = makeStage("root");
		auto mid = makeSub("mid");
		for (int i = 0; i < 3; ++i) mid->add(std::make_shared<SpyItem>("m" + std::to_string(i), spec("S", "r"), &destroyed));
		held = std::make_shared<SpyItem>("held", spec("S", "r"), &destroyed);
		root->add(mid);
		root->add(held);
		root->add(std::make_shared<SpyItem>("direct", spec("S", "r"), &destroyed));
		CHECK(destroyed == 0);
	}
	CHECK(destroyed == 4);                               // everything except the one the caller still holds
	held.reset();
	CHECK(destroyed == 5);
}

// ---------------------------------------------------------------- a whole workflow

TEST_CASE("fork/join pipeline: content branches -> QA sequence -> release") {
	Participant p("Pat", 1, roles("any"));
	auto patch = makeStage("patch", false);
	auto content = makeStage("content", true);
	auto art = makeSub("art"), code = makeSub("code"), audio = makeSub("audio");
	auto model = makeItem("model"), texture = makeItem("texture"), feature = makeItem("feature"), sfx = makeItem("sfx");
	art->add(model); art->add(texture); code->add(feature); audio->add(sfx);
	content->add(art); content->add(code); content->add(audio);
	auto qa = makeStage("qa", false);
	auto integ = makeItem("integration"), build = makeItem("build"), test = makeItem("test");
	qa->add(integ); qa->add(build); qa->add(test);
	auto release = makeItem("release");
	patch->add(content); patch->add(qa); patch->add(release);

	WorkflowSubject subject; Recorder rec; subject.attach(&rec);
	patch->bind(&subject);

	patch->activate();
	CHECK(model->isActive()); CHECK(texture->isActive());
	CHECK(feature->isActive()); CHECK(sfx->isActive());
	CHECK_FALSE(integ->isActive());

	// finishing branches one at a time never opens QA early
	work(*model, p);   patch->activate();
	work(*texture, p); patch->activate();
	CHECK(art->isComplete());
	CHECK_FALSE(integ->isActive());
	work(*feature, p); patch->activate();
	CHECK_FALSE(integ->isActive());
	work(*sfx, p);     patch->activate();
	CHECK(content->isComplete());

	// the join releases QA, which then runs strictly in sequence
	CHECK(integ->isActive());
	CHECK_FALSE(build->isActive());
	work(*integ, p);   patch->activate();
	CHECK(build->isActive());   CHECK_FALSE(test->isActive());
	work(*build, p);   patch->activate();
	CHECK(test->isActive());    CHECK_FALSE(release->isActive());
	work(*test, p);    patch->activate();
	CHECK(qa->isComplete());
	CHECK(release->isActive());
	CHECK_FALSE(patch->isComplete());
	work(*release, p);
	CHECK(patch->isComplete());

	// 8 items x (Available, Assigned, InProgress, Completed)
	CHECK(rec.count("state.changed") == 32);
}

}
