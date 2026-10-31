#include "Task.h"

Task::Task(std::string name, std::string role)
	: name_(std::move(name)), requiredRole_(std::move(role)) {}

const std::string& Task::getName() const { return name_; }

const std::string& Task::getRequiredRole() const { return requiredRole_; }

const std::vector<std::string>& Task::getRequiredFields() const { return requiredFields_; }

void Task::addRequiredField(const std::string& field) { requiredFields_.push_back(field); }

void Task::requireApproval(const std::string& trigger, ApprovalHandler* chain) {
	approvals_[trigger] = chain;
}

bool Task::requiresApproval(const std::string& trigger) const {
	return approvals_.find(trigger) != approvals_.end();
}

ApprovalHandler* Task::getApprovalChain(const std::string& trigger) const {
	std::map<std::string, ApprovalHandler*>::const_iterator it = approvals_.find(trigger);
	return it == approvals_.end() ? nullptr : it->second;
}
