#include "SecuredTask.h"
#include "Participant.h"

SecuredTask::SecuredTask(std::shared_ptr<Task> inner, int requiredClearance)
	: TaskDecorator(std::move(inner)), 
	requiredClearance_(requiredClearance) {}

// [Prototype]
SecuredTask* SecuredTask::clone() const {
	return new SecuredTask(*this);
}

bool SecuredTask::assign(Participant* p) {
	if (!p || p->getAuthority() < requiredClearance_) return false;
	return TaskDecorator::assign(p);
}

bool SecuredTask::complete() {
	Participant* a = getAssignee();
	if (!a || a->getAuthority() < requiredClearance_) return false;
	return TaskDecorator::complete();
}

std::string SecuredTask::describe() const {
	return TaskDecorator::describe() + " [secured L" 
		+ std::to_string(requiredClearance_) + "]";
}
