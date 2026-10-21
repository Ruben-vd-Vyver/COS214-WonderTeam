#include "SecuredTask.h"

SecuredTask::SecuredTask(std::shared_ptr<Task> inner, int requiredClearance)
	: TaskDecorator(std::move(inner)), 
	requiredClearance_(requiredClearance) {}

bool SecuredTask::assign(Participant* p) {
	if (!p || p->clearance < requiredClearance_) return false;
	return TaskDecorator::assign(p);
}

bool SecuredTask::complete() {
	Participant* a = getAssignee();
	if (!a || a->clearance < requiredClearance_) return false;
	return TaskDecorator::complete();
}

std::string SecuredTask::describe() const {
	return TaskDecorator::describe() + " [secured L" 
		+ std::to_string(requiredClearance_) + "]";
}
