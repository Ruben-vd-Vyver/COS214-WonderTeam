#include "WorkflowDefinition.h"
#include <algorithm> //used for find in chains_
#include "Task.h"

WorkflowDefinition::WorkflowDefinition(std::string name, int version) : name_(std::move(name)), version_(version) {}

void WorkflowDefinition::setRoot(std::shared_ptr<WorkflowNode> root)
{
    root_ = std::move(root);
}

std::shared_ptr<WorkflowNode> WorkflowDefinition::getRoot() const
{
    return root_;
}

const std::string &WorkflowDefinition::getName() const
{
    return name_;
}

int WorkflowDefinition::getVersion() const
{
    return version_;
}

void WorkflowDefinition::addTask(std::shared_ptr<Task> t)
{
    if (!t)
        throw "WorkflowDefinition::addTask: task is null";
    tasks_.push_back(std::move(t));
}

const std::vector<std::shared_ptr<Task>> &WorkflowDefinition::getTasks() const
{
    return tasks_;
}

void WorkflowDefinition::addChain(ApprovalHandler *h)
{
    if (!h)
        throw "WorkflowDefinition::addChain: chain is null";
    if (std::find(chains_.begin(), chains_.end(), h) == chains_.end())
        chains_.push_back(h);
}

const std::vector<ApprovalHandler *> &WorkflowDefinition::getChains() const
{
    return chains_;
}

// TODO: once WorkflowInstance return an instance built from root_->clone() (Prototype) so every run gets its own copy of the tree
WorkflowInstance *WorkflowDefinition::createInstance() const
{
    throw "WorkflowDefinition::createInstance: WorkflowInstance not implemented yet";
}
