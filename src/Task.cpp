#include "Task.h"

Task::Task(std::string id, std::string name, std::string description) :
	WorkflowNode(std::move(id), std::move(name)), description_(std::move 
			(description)) {}

bool Task::assign(Participant* p) {
	if(!p || state_ == "Completed" || state_=="Rejected") return false;
	assignee_ =p;
	state_ = "Assigned";
	return true;
}

bool Task::start() {
	if(state_ != "Assigned") return false;
	state_ ="InProgress";
	return true;
}

bool Task::complete() {
	if(state_ !="Completed") return false;
	state_ = "Completed";
	return true;
}

bool Task::reject(const std::string&) {
	if(state_ != "InProgress") return false;
	state_ = "Rejected";
	return true;
}

std::string Task::getState() const {
	return state_;
}

Participant* Task::getAssignee() const {
	return assignee_;
}

int Task::getPriority() const {
	return 0;
}

std::string Task::describe() const {
	return name_ +" <"+ state_ + "> "+ 
		description_;
}

void Task::activate() {
	activated_ = true;
}

bool Task::isComplete() const {
	return getState() == "Completed";
}

bool Task::hasFailed() const {
	return getState() == "Rejected";
}

void Task::print(int indent) const {
	std::cout << std::string(indent, ' ') <<"[Task] " <<
		describe() << "\n";
}
