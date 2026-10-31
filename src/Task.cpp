#include "Task.h"
#include "PendingState.h"

Task::Task(std::string role, std::string id, std::string name, std::string description) : WorkflowNode(std::move(id), std::move(name)), state_(new PendingState()),requiredRole(role), description_(std::move(description)) {}

// [Prototype]
Task::Task(const Task &other)
    : WorkflowNode(other),
      state_(other.state_->clone()),
      activated_(other.activated_),
      assignee_(other.assignee_),
      description_(other.description_) {}

// [Prototype]
Task *Task::clone() const { return new Task(*this); }

bool Task::assign(Participant *p) { return state_->assign(*this, p); }

bool Task::start() { return state_->start(*this); }

bool Task::complete() { return state_->complete(*this); }

bool Task::requestApproval() { return state_->requestApproval(*this); }

bool Task::reject(const std::string &reason)
{
    return state_->reject(*this, reason);
}

bool Task::escalate() { return state_->escalate(*this); }

std::string Task::getState() const { return state_->getName(); }

Participant *Task::getAssignee() const { return assignee_; }

int Task::getPriority() const { return 0; }

std::string Task::describe() const
{
    return name_ + " <" + getState() + "> " + description_;
}

void Task::activate() { state_->activate(*this); }

bool Task::isComplete() const { return state_->isComplete(); }

bool Task::hasFailed() const { return state_->hasFailed(); }

void Task::print(int indent) const
{
    std::cout << std::string(indent, ' ') << "[Task] " << describe() << "\n";
}

