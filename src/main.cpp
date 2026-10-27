#include <iostream>
#include <memory>
#include <set>
#include <string>
#include "Stage.h"
#include "SubProcess.h"
#include "Task.h"
#include "PriorityTask.h"
#include "ValidatedTask.h"
#include "SecuredTask.h"
#include "Participant.h"

// Run: (haven't attempted the makefile yet) 
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
}

int main() {
	std::cout << "TIVIDY - Prototype pattern demo" << std::endl;

	heading("1. Build the template workflow");
	std::shared_ptr<Stage> root(new Stage("root", "Admission", false));
	std::shared_ptr<Task> triage(new Task("t1", "Triage"));

	std::set<std::string> required;
	required.insert("patientId");
	std::shared_ptr<ValidatedTask> vt(new ValidatedTask(std::shared_ptr<Task>(new Task("t2", "Capture details")),required));
	vt->setField("patientId", "P-1");

	std::shared_ptr<Task> signOff(new Task("t3", "Sign off"));
	std::shared_ptr<Task> prio(new PriorityTask(signOff, 5));
	std::shared_ptr<SecuredTask> sec(new SecuredTask(prio, 3));

	std::shared_ptr<SubProcess> sub(new SubProcess("sp", "Billing", 2));
	sub->add(std::shared_ptr<Task>(new Task("t4", "Invoice")));

	root->add(triage);
	root->add(vt);
	root->add(sec);
	root->add(sub);

	std::cout << "Template:" << std::endl;root->print(2);

	heading("2. Clone the template (createInstance)");
	std::shared_ptr<WorkflowNode> inst(root->clone());
	std::cout << "Instance:" << std::endl;
	inst->print(2);

	check(inst.get() != root.get(), "the instance is a different object from the template");
	check(dynamic_cast<Stage*>(inst.get()) != nullptr, "the clone is still a Stage (type kept)");
	check(inst->getChildren().size() == 4, "the clone has all 4 children");

	bool sameIds = true, differentObjects = true;
	for (std::size_t i = 0; i < inst->getChildren().size(); ++i) {
		if (inst->getChildren()[i]->getId() != root->getChildren()[i]->getId())
			sameIds = false;
		if (inst->getChildren()[i] == root->getChildren()[i])
			differentObjects = false;
	}
	check(sameIds, "children keep the same ids");
	check(differentObjects, "children are new objects, not shared with the template");

	SubProcess* subCopy = dynamic_cast<SubProcess*>(inst->getChild("sp").get());
	check(subCopy && subCopy->getSourceVersion() == 2, "SubProcess keeps its sourceVersion");
	check(subCopy && subCopy->getChild("t4") && subCopy->getChild("t4") != sub->getChild("t4"),"nested children are deep-copied too");

	SecuredTask* secCopy = dynamic_cast<SecuredTask*>(inst->getChild("t3").get());
	check(secCopy && secCopy != sec.get(), "decorated task cloned as a SecuredTask");
	check(secCopy && secCopy->getPriority() == 5, "decorator chain kept (priority 5 under security)");

	ValidatedTask* vtCopy = dynamic_cast<ValidatedTask*>(inst->getChild("t2").get());
	check(vtCopy && vtCopy->missingFields().empty(), "ValidatedTask field values were copied");

	heading("3. Run the instance and check the template is untouched");
	Participant boss("Dr Boss", 5, std::vector<std::string>(1, "doctor"));
	std::cout << "Participant: " << boss.describe() << std::endl;

	inst->activate();
	Task* triageCopy = dynamic_cast<Task*>(inst->getChild("t1").get());
	bool assigned = triageCopy->assign(&boss);
	bool started = triageCopy->start();
	bool completed = triageCopy->complete();
	std::cout << "Instance triage: assign=" << assigned << " start=" << started << " complete=" << completed << " -> " << triageCopy->getState() << std::endl;
	check(triageCopy->getState() == "Completed", "instance triage task is Completed");
	check(triage->getState() == "Pending", "template triage task is still Pending");

	secCopy->activate();
	check(secCopy->assign(&boss), "instance sign-off assigned to Dr Boss (authority 5 >= 3)");
	check(sec->getAssignee() == nullptr && sec->getState() == "Pending","template sign-off still unassigned and Pending");

	std::cout << "\nTemplate after running the instance:" << std::endl;
	root->print(2);
	std::cout << "Instance after running:" << std::endl;
	inst->print(2);

	heading("4. Clone mid-run (snapshot of current state)");
	std::unique_ptr<Task> snap(triageCopy->clone());
	std::cout << "Snapshot: ";
	snap->print();
	check(snap->getState() == "Completed", "snapshot copied the current state");
	check(snap->getAssignee() == &boss, "snapshot kept the assignee");

	std::cout << "\n" << passed << " passed, " << failed << " failed" << std::endl;
	return failed == 0 ? 0 : 1;
}
