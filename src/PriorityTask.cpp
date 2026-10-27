#include "PriorityTask.h"

PriorityTask::PriorityTask(std::shared_ptr<Task> inner, int priority) 
	: TaskDecorator(std::move(inner)), priority_(priority) {}

// [Prototype]
PriorityTask* PriorityTask::clone() const {
	return new PriorityTask(*this);
}

int PriorityTask::getPriority() const {
	return priority_;
}

void PriorityTask::setPriority(int p) {
	priority_ =p;
}

std::string PriorityTask::describe() const {
	return TaskDecorator::describe() +" [priority "+std::to_string(
			priority_) + "]";
}
