#include "TaskDecorator.h"

TaskDecorator::TaskDecorator(std::shared_ptr<Task> inner) 
	: Task(inner? inner->getId() : throw std::invalid_argument(
				"decorator needs a task"),
			inner->getName()), inner_(std::move(inner)) {}

bool TaskDecorator::assign(Participant* p) {
	return inner_->assign(p);
}

bool TaskDecorator::start() {
	return inner_->start();
}

bool TaskDecorator::complete() {
	return inner_->complete();
}

bool TaskDecorator::reject(const std::string& r) {
	return inner_->reject(r);
}

std::string TaskDecorator::getState() const {
	return inner_->getState();
}

Participant* TaskDecorator::getAssignee() const {
	return inner_->getAssignee();
}

int TaskDecorator::getPriority() const {
	return inner_->getPriority();
}

std::string TaskDecorator::describe() const {
	return inner_->describe();
}

void TaskDecorator::activate() {
	inner_->activate();
}

bool TaskDecorator::isComplete() const {
	return inner_->isComplete();
}

bool TaskDecorator::hasFailed() const {
	return inner_->hasFailed();
}
