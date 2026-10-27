#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "Stage.h"
#include "SubProcess.h"
#include "Task.h"
#include "Participant.h"
#include "WorkIterator.h"

// Run: (no makefile yet)
// g++ -std=c++11 -Iinclude src/*.cpp -o tividy
// ./tividy

namespace {
	int passed = 0;
	int failed = 0;

	void check(bool ok, const std::string& what) {
		std::cout << (ok ? "  [PASS] " : "  [FAIL] ") << what << std::endl;
		if (ok) ++passed; else ++failed;
	}

	void heading(const std::string& title) {
		std::cout << "\n=== " << title << " ===" << std::endl;
	}

	// Every node below `node`, in iteration order, as "id" strings.
	std::vector<std::string> visitOrder(WorkflowNode* node) {
		std::vector<std::string> ids;
		WorkIterator* it = node->createIterator();
		for (it->first(); !it->isDone(); it->next())
			ids.push_back(it->current()->getId());
		delete it;
		return ids;
	}

	// Example client: find a task by id (what WorkflowInstance::findItem can do) without knowing the shape of the tree.
	Task* findTask(WorkflowNode* root, const std::string& id) {
		Task* found = nullptr;
		WorkIterator* it = root->createIterator();
		for (it->first(); !it->isDone() && !found; it->next())
			if (it->current()->getId() == id)
				found = dynamic_cast<Task*>(it->current());
		delete it;
		return found;
	}

	// Example client: the "list available work" view for the TUI.
	std::vector<Task*> availableWork(WorkflowNode* root) {
		std::vector<Task*> work;
		WorkIterator* it = root->createIterator();
		for (it->first(); !it->isDone(); it->next()) {
			Task* t = dynamic_cast<Task*>(it->current());
			if (t && t->getState() == "Available") work.push_back(t);
		}
		delete it;
		return work;
	}

	void printWork(const std::vector<Task*>& work) {
		std::cout << "Available work (" << work.size() << "):" << std::endl;
		for (std::size_t i = 0; i < work.size(); ++i)
			std::cout << "  - " << work[i]->getId() << " " << work[i]->getName() << std::endl;
	}

	std::string join(const std::vector<std::string>& v) {
		std::string out;
		for (std::size_t i = 0; i < v.size(); ++i) out += (i ? " -> " : "") + v[i];
		return out.empty() ? "(nothing)" : out;
	}
}

int main() {
	std::cout << "TIVIDY - Iterator pattern demo" << std::endl;

	heading("1. Build a workflow");
	std::shared_ptr<Stage> root(new Stage("root", "Admission", false));
	std::shared_ptr<Task> triage(new Task("t1", "Triage"));
	std::shared_ptr<Stage> assess(new Stage("assess", "Assessment", true));
	assess->add(std::shared_ptr<Task>(new Task("t2", "Blood test")));
	assess->add(std::shared_ptr<Task>(new Task("t3", "X-ray")));
	std::shared_ptr<SubProcess> billing(new SubProcess("sp", "Billing", 1));
	billing->add(std::shared_ptr<Task>(new Task("t4", "Invoice")));
	std::shared_ptr<Task> discharge(new Task("t5", "Discharge"));

	root->add(triage);
	root->add(assess);
	root->add(billing);
	root->add(discharge);
	root->print(2);

	heading("2. Depth-first traversal of the whole tree");
	std::vector<std::string> order = visitOrder(root.get());
	std::cout << "Visit order: " << join(order) << std::endl;
	std::vector<std::string> expected;
	expected.push_back("t1"); expected.push_back("assess"); expected.push_back("t2");
	expected.push_back("t3"); expected.push_back("sp"); expected.push_back("t4");
	expected.push_back("t5");
	check(order == expected, "nodes visited top-to-bottom, going into each child before the next sibling");

	WorkIterator* it = root->createIterator();
	int firstPass = 0, secondPass = 0;
	for (it->first(); !it->isDone(); it->next()) ++firstPass;
	for (it->first(); !it->isDone(); it->next()) ++secondPass;
	check(firstPass == 7 && secondPass == 7, "first() restarts the same iterator (7 nodes both times)");
	check(it->current() == nullptr, "current() is nullptr once the iterator is done");
	delete it;

	heading("3. Iterating part of the tree and leaves");
	std::cout << "Below Billing:    " << join(visitOrder(billing.get())) << std::endl;
	check(visitOrder(billing.get()) == std::vector<std::string>(1, "t4"), "iterating a SubProcess only visits its own subtree");

	WorkIterator* leafIt = triage->createIterator();
	leafIt->first();
	std::cout << "Below Triage:     " << join(visitOrder(triage.get())) << std::endl;
	check(dynamic_cast<WorkIterator*>(leafIt) && leafIt->isDone() && leafIt->current() == nullptr, "a Task gives a NullIterator that is done straight away");
	delete leafIt;

	Stage empty("empty", "Empty stage");
	std::cout << "Below Empty stage: " << join(visitOrder(&empty)) << std::endl;
	check(visitOrder(&empty).empty(), "an empty Stage has nothing to iterate");

	heading("4. Clients using the iterator");
	Task* xray = findTask(root.get(), "t3");
	check(xray && xray->getName() == "X-ray", "findTask(\"t3\") found X-ray inside the parallel stage");
	check(findTask(root.get(), "nope") == nullptr, "findTask on an unknown id returns nullptr");

	Participant nurse("Nurse Ann", 2, std::vector<std::string>(1, "nurse"));

	std::cout << "\nActivating the workflow (sequential, so only the first task opens)" << std::endl;
	root->activate();
	std::vector<Task*> work = availableWork(root.get());
	printWork(work);
	check(work.size() == 1 && work[0]->getId() == "t1", "only Triage is available");

	std::cout << "\nNurse Ann does Triage, then the workflow moves on" << std::endl;
	triage->assign(&nurse);
	triage->start();
	triage->complete();
	root->activate();
	work = availableWork(root.get());
	printWork(work);
	check(work.size() == 2 && work[0]->getId() == "t2" && work[1]->getId() == "t3", "the parallel Assessment stage opened both of its tasks");

	std::cout << "\nWorkflow now:" << std::endl;
	root->print(2);

	std::cout << "\n" << passed << " passed, " << failed << " failed" << std::endl;
	return failed == 0 ? 0 : 1;
}
