#include "WorkItemDecorator.h"
#include <stdexcept>

namespace {
	WorkItem* require(WorkItem* w) {
    	if (!w) throw std::invalid_argument("a decorator needs a WorkItem to wrap");
	return w;
	}
}

WorkItemDecorator::WorkItemDecorator(WorkItem* inner)
    : WorkItem(require(inner)->getId(), inner->getName()), inner_(inner) {}

WorkItemDecorator::~WorkItemDecorator() { 
	delete inner_; 
}

void WorkItemDecorator::assign(Participant* p) {
	inner_->assign(p);
}

void WorkItemDecorator::start() {
	inner_->start(); 
}

void WorkItemDecorator::complete() {
	inner_->complete(); 
}

void WorkItemDecorator::reject(const std::string& r) { 
	inner_->reject(r);
}

void WorkItemDecorator::escalate() {
	inner_->escalate();
}

void WorkItemDecorator::approve() {
	inner_->approve();
}

void WorkItemDecorator::activate() {
	inner_->activate();
}

bool WorkItemDecorator::isComplete() const {
	return inner_->isComplete();
}

bool WorkItemDecorator::isActive() const { 
	return inner_->isActive();
}

const Task& WorkItemDecorator::getTask() const {
	return inner_->getTask();
}

std::string WorkItemDecorator::getStateName() const {
	return inner_->getStateName();
}

std::string WorkItemDecorator::getRequiredRole() const {
	return inner_->getRequiredRole();
}

Participant* WorkItemDecorator::getAssignee() const {
	return inner_->getAssignee();
}

void WorkItemDecorator::setField(const std::string& k, const std::string& v) {
	inner_->setField(k, v); 
}

std::string WorkItemDecorator::getField(const std::string& k) const {
	return inner_->getField(k);
}

void WorkItemDecorator::bind(WorkflowSubject* s) {
	inner_->bind(s);
}

std::unique_ptr<WorkItemMemento> WorkItemDecorator::createMemento() const {
	return inner_->createMemento();
}

void WorkItemDecorator::restore(const WorkItemMemento& m) {
	inner_->restore(m);
}

