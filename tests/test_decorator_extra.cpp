// Thorough Decorator tests: forwarding, each concrete decorator, stacking, cloning, ownership, memento, events.
#include <algorithm>
#include "doctest.h"
#include "test_tree_helpers.h"
#include "PriorityItem.h"
#include "SecuredItem.h"
#include "ValidatedItem.h"

using namespace testutil;

TEST_SUITE("Decorator (thorough)") {

// ---------------------------------------------------------------- the base decorator: plain forwarding

TEST_CASE("a decorator wears the identity of the item it wraps") {
	std::shared_ptr<Task> t = spec("Dose", "nurse");
	BasicWorkItem* inner = new BasicWorkItem("d1", t);
	PriorityItem item(inner, 1);
	CHECK(item.getId() == "d1");
	CHECK(item.getName() == "Dose");
	CHECK(&item.getTask() == t.get());
	CHECK(item.getRequiredRole() == "nurse");
	CHECK(item.asWorkItem() == &item);                  // the decorator, not the thing inside, is the work item the tree sees
	CHECK(item.childCount() == 0);
	CHECK(visitOrder(&item) == "");
}

TEST_CASE("a decorator needs something to wrap") {
	CHECK_THROWS_AS(PriorityItem(nullptr, 1), std::invalid_argument);
	CHECK_THROWS_AS(SecuredItem(nullptr, roles("x")), std::invalid_argument);
	CHECK_THROWS_AS(ValidatedItem(nullptr), std::invalid_argument);
}

TEST_CASE("every operation is forwarded to the wrapped item") {
	Participant ann("Ann", 1, roles("nurse"));
	BasicWorkItem* inner = new BasicWorkItem("f", spec("Fwd", "nurse"));
	PriorityItem item(inner, 2);                        // does not interfere with anything except assign()

	CHECK(item.getStateName() == "Pending");
	CHECK_FALSE(item.isActive());
	item.activate();
	CHECK(inner->getStateName() == "Available");        // the INNER item moved
	CHECK(item.getStateName() == "Available");
	CHECK(item.isActive());

	item.assign(&ann);
	CHECK(inner->getAssignee() == &ann);
	CHECK(item.getAssignee() == &ann);
	CHECK(item.getStateName() == "Assigned");

	item.start();
	CHECK(inner->getStateName() == "InProgress");

	item.setField("k", "v");
	CHECK(inner->getField("k") == "v");
	CHECK(item.getField("k") == "v");
	inner->setField("direct", "yes");
	CHECK(item.getField("direct") == "yes");
	CHECK(item.getField("missing") == "");

	CHECK_FALSE(item.isComplete());
	item.complete();
	CHECK(inner->isComplete());
	CHECK(item.isComplete());
	CHECK_FALSE(item.isActive());
}

TEST_CASE("reject and escalate are forwarded and the lifecycle rules still apply") {
	Participant ann("Ann", 1, roles("nurse"));

	SUBCASE("reject from InProgress") {
		PriorityItem item(new BasicWorkItem("r", spec("R", "nurse")), 1);
		item.activate(); item.assign(&ann); item.start();
		item.reject("not needed");
		CHECK(item.getStateName() == "Rejected");
		CHECK_FALSE(item.isComplete());
		CHECK_FALSE(item.isActive());
	}
	SUBCASE("escalate from Assigned") {
		PriorityItem item(new BasicWorkItem("e", spec("E", "nurse")), 1);
		item.activate(); item.assign(&ann);
		item.escalate();
		CHECK(item.getStateName() == "Escalated");
		CHECK_FALSE(item.isComplete());
	}
}

TEST_CASE("illegal transitions throw through the decorator and change nothing") {
	Participant ann("Ann", 1, roles("nurse"));
	PriorityItem item(new BasicWorkItem("x", spec("X", "nurse")), 1);

	CHECK_THROWS_AS(item.assign(&ann), std::logic_error);     // Pending: not opened yet
	CHECK_THROWS_AS(item.start(), std::logic_error);
	CHECK_THROWS_AS(item.complete(), std::logic_error);
	CHECK_THROWS_AS(item.approve(), std::logic_error);
	CHECK(item.getStateName() == "Pending");
	CHECK(item.getAssignee() == nullptr);

	item.activate();
	CHECK_THROWS_AS(item.complete(), std::logic_error);       // Available: not started
	CHECK_THROWS_AS(item.start(), std::logic_error);          // Available: nobody assigned
	CHECK(item.getStateName() == "Available");

	item.assign(&ann); item.start(); item.complete();
	CHECK_THROWS_AS(item.complete(), std::logic_error);       // Completed is final
	CHECK_THROWS_AS(item.start(), std::logic_error);
	CHECK_THROWS_AS(item.reject("late"), std::logic_error);
	CHECK_THROWS_AS(item.escalate(), std::logic_error);
	CHECK(item.getStateName() == "Completed");
}

// ---------------------------------------------------------------- PriorityItem

TEST_CASE("PriorityItem stamps its priority on every assignment") {
	Participant ann("Ann", 1, roles("nurse"));
	Participant bob("Bob", 1, roles("nurse"));

	SUBCASE("the value is stored and can be changed") {
		PriorityItem item(new BasicWorkItem("p", spec("P", "nurse")), 8);
		CHECK(item.getPriority() == 8);
		item.setPriority(2);
		CHECK(item.getPriority() == 2);
		item.activate(); item.assign(&ann);
		CHECK(item.getField("priority") == "2");
	}
	SUBCASE("zero and negative priorities work") {
		PriorityItem zero(new BasicWorkItem("z", spec("Z", "nurse")), 0);
		zero.activate(); zero.assign(&ann);
		CHECK(zero.getField("priority") == "0");
		PriorityItem neg(new BasicWorkItem("n", spec("N", "nurse")), -5);
		neg.activate(); neg.assign(&bob);
		CHECK(neg.getField("priority") == "-5");
	}
	SUBCASE("nothing is stamped before assignment") {
		PriorityItem item(new BasicWorkItem("q", spec("Q", "nurse")), 5);
		item.activate();
		CHECK(item.getField("priority") == "");
	}
	SUBCASE("a failed assignment stamps nothing") {
		PriorityItem item(new BasicWorkItem("f", spec("F", "nurse")), 5);
		CHECK_THROWS_AS(item.assign(&ann), std::logic_error);      // still Pending
		CHECK(item.getField("priority") == "");
	}
}

// ---------------------------------------------------------------- ValidatedItem

TEST_CASE("ValidatedItem lists exactly the fields that are still missing") {
	std::shared_ptr<Task> form = spec("Form", "nurse");
	form->addRequiredField("a");
	form->addRequiredField("b");
	form->addRequiredField("c");
	ValidatedItem v(new BasicWorkItem("v", form));

	std::vector<std::string> all = {"a", "b", "c"};
	CHECK(v.missingFields() == all);
	v.setField("b", "x");
	std::vector<std::string> ac = {"a", "c"};
	CHECK(v.missingFields() == ac);
	v.setField("b", "");                                // blanking a field makes it missing again
	CHECK(v.missingFields() == all);
	v.setField("a", "1"); v.setField("b", "2"); v.setField("c", "3");
	CHECK(v.missingFields().empty());
	v.setField("unrelated", "zzz");                     // extra fields are fine
	CHECK(v.missingFields().empty());
}

TEST_CASE("ValidatedItem refuses to complete, changes nothing, and succeeds on retry") {
	Participant ann("Ann", 1, roles("nurse"));
	std::shared_ptr<Task> form = spec("Form", "nurse");
	form->addRequiredField("id");
	form->addRequiredField("dose");
	ValidatedItem v(new BasicWorkItem("v", form));
	v.activate(); v.assign(&ann); v.start();

	CHECK_THROWS_AS(v.complete(), std::runtime_error);
	CHECK(v.getStateName() == "InProgress");
	v.setField("id", "P-1");
	CHECK_THROWS_AS(v.complete(), std::runtime_error);   // still missing "dose"
	CHECK(v.getStateName() == "InProgress");
	CHECK_FALSE(v.isComplete());
	v.setField("dose", "5mg");
	CHECK_NOTHROW(v.complete());
	CHECK(v.isComplete());
}

TEST_CASE("ValidatedItem only guards complete()") {
	Participant ann("Ann", 1, roles("nurse"));
	std::shared_ptr<Task> form = spec("Form", "nurse");
	form->addRequiredField("id");

	SUBCASE("start, escalate and reject ignore missing fields") {
		ValidatedItem a(new BasicWorkItem("a", form));
		a.activate(); a.assign(&ann);
		CHECK_NOTHROW(a.start());
		CHECK_NOTHROW(a.reject("cancelled"));
		ValidatedItem b(new BasicWorkItem("b", form));
		b.activate(); b.assign(&ann);
		CHECK_NOTHROW(b.escalate());
	}
	SUBCASE("with nothing required it behaves like a plain item") {
		ValidatedItem c(new BasicWorkItem("c", spec("Plain", "nurse")));
		CHECK(c.missingFields().empty());
		c.activate(); c.assign(&ann); c.start();
		CHECK_NOTHROW(c.complete());
	}
	SUBCASE("with nothing required, the lifecycle rules still apply") {
		ValidatedItem d(new BasicWorkItem("d", spec("Plain", "nurse")));
		CHECK_THROWS_AS(d.complete(), std::logic_error);  // not a runtime_error: validation passed, the state said no
	}
}

TEST_CASE("ValidatedItem reads the Task spec every time, so later requirements count") {
	Participant ann("Ann", 1, roles("nurse"));
	std::shared_ptr<Task> form = spec("Form", "nurse");
	ValidatedItem v(new BasicWorkItem("v", form));
	v.activate(); v.assign(&ann); v.start();
	CHECK(v.missingFields().empty());
	form->addRequiredField("late");                     // the spec is shared and read-only to the item, but not frozen
	CHECK(v.missingFields().size() == 1);
	CHECK_THROWS_AS(v.complete(), std::runtime_error);
	v.setField("late", "ok");
	CHECK_NOTHROW(v.complete());
}

TEST_CASE("fields filled in on the wrapped item count too") {
	Participant ann("Ann", 1, roles("nurse"));
	std::shared_ptr<Task> form = spec("Form", "nurse");
	form->addRequiredField("id");
	BasicWorkItem* inner = new BasicWorkItem("v", form);
	ValidatedItem v(inner);
	v.activate(); v.assign(&ann); v.start();
	inner->setField("id", "P-9");                       // set behind the decorator's back
	CHECK(v.missingFields().empty());
	CHECK_NOTHROW(v.complete());
}

// ---------------------------------------------------------------- SecuredItem

TEST_CASE("SecuredItem admits any participant who holds a permitted role") {
	Participant nurse("Nia", 1, roles("nurse"));
	Participant porter("Pete", 1, roles("porter"));
	std::vector<std::string> two = {"nurse", "doctor"};
	std::vector<std::string> acceptsBoth = {"nurse", "doctor"};

	SUBCASE("rejected participants change nothing") {
		SecuredItem s(new BasicWorkItem("s", spec("S", "nurse")), two);
		s.activate();
		CHECK_THROWS_AS(s.assign(&porter), std::runtime_error);
		CHECK(s.getStateName() == "Available");
		CHECK(s.getAssignee() == nullptr);
		CHECK_NOTHROW(s.assign(&nurse));                // and the next, permitted person is still accepted
		CHECK(s.getAssignee() == &nurse);
	}
	SUBCASE("a participant with several roles needs only one of them to match") {
		Participant multi("Max", 1, roles("porter"));
		multi.addRole("doctor");
		SecuredItem s(new BasicWorkItem("m", spec("M", "nurse")), acceptsBoth);
		s.activate();
		CHECK_NOTHROW(s.assign(&multi));
	}
	SUBCASE("an empty permission list admits nobody") {
		SecuredItem s(new BasicWorkItem("e", spec("E", "nurse")), std::vector<std::string>());
		s.activate();
		CHECK_THROWS_AS(s.assign(&nurse), std::runtime_error);
		CHECK_THROWS_AS(s.assign(&porter), std::runtime_error);
	}
}

TEST_CASE("SecuredItem will not start work for an unpermitted assignee") {
	Participant nurse("Nia", 1, roles("nurse"));
	Participant porter("Pete", 1, roles("porter"));

	SUBCASE("someone slipped in behind the decorator's back") {
		BasicWorkItem* inner = new BasicWorkItem("s", spec("S", "nurse"));
		SecuredItem s(inner, roles("nurse"));
		s.activate();
		inner->assign(&porter);                          // bypasses SecuredItem::assign
		CHECK_THROWS_AS(s.start(), std::runtime_error);
		CHECK(s.getStateName() == "Assigned");
	}
	SUBCASE("nobody assigned at all") {
		SecuredItem s(new BasicWorkItem("s", spec("S", "nurse")), roles("nurse"));
		s.activate();
		CHECK_THROWS_AS(s.start(), std::runtime_error);
	}
	SUBCASE("the permitted path works end to end") {
		SecuredItem s(new BasicWorkItem("s", spec("S", "nurse")), roles("nurse"));
		s.activate(); s.assign(&nurse);
		CHECK_NOTHROW(s.start());
		CHECK_NOTHROW(s.complete());
		CHECK(s.isComplete());
	}
}

// ---------------------------------------------------------------- stacking

TEST_CASE("any stacking order enforces all three rules") {
	Participant ann("Ann", 1, roles("nurse"));
	Participant bob("Bob", 1, roles("porter"));

	std::string order = "PSV";
	std::sort(order.begin(), order.end());
	do {
		INFO("stacking order (inner -> outer): " << order);
		std::shared_ptr<Task> chart = spec("Chart", "nurse");
		chart->addRequiredField("dose");

		WorkItem* w = new BasicWorkItem("x", chart);
		for (size_t i = 0; i < order.size(); ++i) {
			if (order[i] == 'P') w = new PriorityItem(w, 5);
			if (order[i] == 'S') w = new SecuredItem(w, roles("nurse"));
			if (order[i] == 'V') w = new ValidatedItem(w);
		}
		std::unique_ptr<WorkItem> item(w);

		CHECK(item->getId() == "x");
		item->activate();
		CHECK_THROWS_AS(item->assign(&bob), std::runtime_error);    // Secured
		CHECK(item->getStateName() == "Available");
		CHECK(item->getField("priority") == "");                    // a refused assignment is not stamped
		item->assign(&ann);
		CHECK(item->getField("priority") == "5");                   // Priority
		item->start();
		CHECK_THROWS_AS(item->complete(), std::runtime_error);      // Validated
		CHECK(item->getStateName() == "InProgress");
		item->setField("dose", "5mg");
		item->complete();
		CHECK(item->isComplete());
	} while (std::next_permutation(order.begin(), order.end()));
}

TEST_CASE("the same decorator can be applied more than once") {
	Participant ann("Ann", 1, roles("nurse"));
	Participant doc("Dee", 1, roles("doctor"));
	// two locks: you need to satisfy BOTH
	std::vector<std::string> nurseOrDoctor = {"nurse", "doctor"};
	SecuredItem item(new SecuredItem(new BasicWorkItem("t", spec("T", "nurse")), nurseOrDoctor), roles("nurse"));
	item.activate();
	CHECK_THROWS_AS(item.assign(&doc), std::runtime_error);     // outer lock refuses
	CHECK_NOTHROW(item.assign(&ann));
}

TEST_CASE("a deep stack forwards correctly, clones, and cleans up") {
	Participant ann("Ann", 1, roles("nurse"));
	WorkItem* w = new BasicWorkItem("deep", spec("Deep", "nurse"));
	for (int i = 0; i < 10; ++i) w = new PriorityItem(w, i);
	std::unique_ptr<WorkItem> item(w);

	item->activate();
	item->assign(&ann);
	CHECK(item->getField("priority") == "9");           // inner layers stamp first, the outermost has the last word
	item->start();

	std::unique_ptr<WorkItem> copy(item->clone());
	CHECK(copy->getField("priority") == "9");
	copy->complete();
	CHECK(copy->isComplete());
	CHECK(item->getStateName() == "InProgress");
}

// ---------------------------------------------------------------- clone()

TEST_CASE("clones keep their rules and are independent of the original") {
	Participant ann("Ann", 1, roles("nurse"));
	Participant bob("Bob", 1, roles("porter"));
	std::shared_ptr<Task> chart = spec("Chart", "nurse");
	chart->addRequiredField("dose");

	SecuredItem original(new ValidatedItem(new PriorityItem(new BasicWorkItem("c", chart), 6)), roles("nurse"));
	std::unique_ptr<WorkItem> copy(original.clone());

	REQUIRE(copy.get() != &original);
	CHECK(dynamic_cast<SecuredItem*>(copy.get()) != nullptr);
	CHECK(copy->getId() == "c");

	// the copy enforces the same three rules
	copy->activate();
	CHECK_THROWS_AS(copy->assign(&bob), std::runtime_error);
	copy->assign(&ann);
	CHECK(copy->getField("priority") == "6");
	copy->start();
	CHECK_THROWS_AS(copy->complete(), std::runtime_error);
	copy->setField("dose", "1mg");
	copy->complete();

	// the original has not moved
	CHECK(original.getStateName() == "Pending");
	CHECK(original.getAssignee() == nullptr);
	CHECK(original.getField("dose") == "");
}

TEST_CASE("a clone is independent in the other direction too") {
	Participant ann("Ann", 1, roles("nurse"));
	PriorityItem original(new BasicWorkItem("o", spec("O", "nurse")), 7);
	std::unique_ptr<WorkItem> copy(original.clone());
	PriorityItem* pc = dynamic_cast<PriorityItem*>(copy.get());
	REQUIRE(pc != nullptr);

	original.setPriority(1);
	CHECK(pc->getPriority() == 7);
	pc->setPriority(3);
	CHECK(original.getPriority() == 1);

	original.activate(); original.assign(&ann);
	CHECK(copy->getStateName() == "Pending");
	CHECK(copy->getAssignee() == nullptr);
}

TEST_CASE("cloning mid-run copies the state, assignee and fields through every layer") {
	Participant ann("Ann", 1, roles("nurse"));
	SecuredItem original(new PriorityItem(new BasicWorkItem("m", spec("M", "nurse")), 4), roles("nurse"));
	original.activate(); original.assign(&ann); original.start();
	original.setField("note", "keep me");

	std::unique_ptr<WorkItem> copy(original.clone());
	CHECK(copy->getStateName() == "InProgress");
	CHECK(copy->getAssignee() == &ann);
	CHECK(copy->getField("note") == "keep me");
	CHECK(copy->getField("priority") == "4");
	CHECK(&copy->getTask() == &original.getTask());     // the read-only spec is shared, not copied
}

// ---------------------------------------------------------------- ownership

TEST_CASE("destroying a decorator destroys what it wraps, exactly once") {
	int destroyed = 0;
	{
		std::unique_ptr<WorkItem> w(new SecuredItem(
			new PriorityItem(new ValidatedItem(new SpyItem("s", spec("S", "r"), &destroyed)), 1),
			roles("r")));
		CHECK(destroyed == 0);
	}
	CHECK(destroyed == 1);
}

TEST_CASE("a decorator takes ownership of the item it is given") {
	int destroyed = 0;
	SpyItem* inner = new SpyItem("s", spec("S", "r"), &destroyed);
	{
		PriorityItem item(inner, 1);
		CHECK(destroyed == 0);
	}
	CHECK(destroyed == 1);
}

// ---------------------------------------------------------------- memento

TEST_CASE("a memento taken through a decorator restores state, assignee and fields") {
	Participant ann("Ann", 1, roles("nurse"));
	PriorityItem item(new BasicWorkItem("m", spec("M", "nurse")), 4);
	item.activate(); item.assign(&ann); item.start();
	item.setField("note", "before");

	auto snapshot = item.createMemento();
	item.setField("note", "after");
	item.complete();
	REQUIRE(item.isComplete());

	item.restore(*snapshot);
	CHECK(item.getStateName() == "InProgress");
	CHECK(item.getAssignee() == &ann);
	CHECK(item.getField("note") == "before");
	CHECK(item.getField("priority") == "4");
	CHECK_NOTHROW(item.complete());                     // work can carry on from the restored point
}

TEST_CASE("a snapshot can be restored more than once") {
	Participant ann("Ann", 1, roles("nurse"));
	PriorityItem item(new BasicWorkItem("m", spec("M", "nurse")), 1);
	item.activate();
	auto atAvailable = item.createMemento();
	item.assign(&ann); item.start(); item.complete();

	item.restore(*atAvailable);
	CHECK(item.getStateName() == "Available");
	item.assign(&ann); item.start();
	item.restore(*atAvailable);
	CHECK(item.getStateName() == "Available");
	CHECK(item.getAssignee() == nullptr);
}

// ---------------------------------------------------------------- events

TEST_CASE("a bound decorated item publishes its state changes") {
	Participant ann("Ann", 1, roles("nurse"));
	Participant bob("Bob", 1, roles("porter"));
	WorkflowSubject subject; Recorder rec; subject.attach(&rec);

	SecuredItem item(new PriorityItem(new BasicWorkItem("o", spec("O", "nurse")), 2), roles("nurse"));
	item.bind(&subject);
	item.activate();
	item.assign(&ann);
	std::vector<std::string> want = {"Available", "Assigned"};
	CHECK(rec.details == want);
	CHECK(rec.itemIds[0] == "o");

	size_t before = rec.types.size();
	CHECK_THROWS_AS(item.assign(&bob), std::runtime_error);     // refused by the decorator: no event
	CHECK(rec.types.size() == before);
	CHECK_THROWS_AS(item.complete(), std::logic_error);          // refused by the state: no event
	CHECK(rec.types.size() == before);
}

TEST_CASE("an unbound decorated item is silent") {
	Participant ann("Ann", 1, roles("nurse"));
	WorkflowSubject subject; Recorder rec; subject.attach(&rec);
	PriorityItem item(new BasicWorkItem("q", spec("Q", "nurse")), 2);
	item.activate(); item.assign(&ann);
	CHECK(rec.types.empty());
}

}
