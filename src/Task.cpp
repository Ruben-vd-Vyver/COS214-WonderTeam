#include "Task.h"

Task::Task(std::string id, std::string name, std::string description) :
	WorkflowNode(std::move(id), std::move(name)), description_(std::move 
			(description)) {}

bool Task::assign(Participant* p) {
	if(!p || state_ == "Completed" || state_=="Rejected") return false;
	assignees_.push_back(p);
	state_ = "Assigned";
	return true;
}

bool Task::unassign(Participant* p){
	if(!p || state_ == "Completed" || state_=="Rejected") return false;
	auto newEnd = std::remove(assignees_.begin(), assignees_.end(), p);
	if (newEnd == assignees_.end()) return false;
	assignees_.erase(newEnd, assignees_.end());
	if (assignees_.empty())
		state_ = "Pending";

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

std::vector<Participant*> Task::getAssignees() const {
	return assignees_;
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
