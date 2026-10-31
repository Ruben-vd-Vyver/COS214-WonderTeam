#include "Task.h"

// Creates a task specification with a name and the role that performs it
Task::Task(std::string name, std::string role) : name_(std::move(name)), requiredRole_(std::move(role)) {}

// Returns the task name
const std::string &Task::getName() const
{
    return name_;
}

// Returns the role required to perform the task
const std::string &Task::getRequiredRole() const
{
    return requiredRole_;
}

// Declares a field that must be filled in before the task can complete
void Task::addRequiredField(const std::string &field)
{
    if (field.empty())
        throw std::string("Task::addRequiredField: field name is empty");
    requiredFields_.push_back(field);
}

// Returns the required fields in declaration order
const std::vector<std::string> &Task::getRequiredFields() const
{
    return requiredFields_;
}

// Attaches an approval chain to a trigger, replacing any earlier one
void Task::requireApproval(const std::string &trigger, ApprovalHandler *chain)
{
    if (!chain)
        throw std::string("Task::requireApproval: chain is null");
    approvals_[trigger] = chain;
}

// Reports whether a trigger needs approval
bool Task::requiresApproval(const std::string &trigger) const
{
    return approvals_.find(trigger) != approvals_.end();
}

// Returns the chain for a trigger, or nullptr when none is required
ApprovalHandler *Task::getApprovalChain(const std::string &trigger) const
{
    std::map<std::string, ApprovalHandler *>::const_iterator it = approvals_.find(trigger);
    return it == approvals_.end() ? nullptr : it->second;
}
