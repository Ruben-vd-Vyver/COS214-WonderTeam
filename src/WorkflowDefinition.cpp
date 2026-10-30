#include "WorkflowDefinition.h"
#include <algorithm> //used for find in chains_
#include "Task.h"

// Constructs a workflow definition with the supplied name and version
WorkflowDefinition::WorkflowDefinition(std::string name, int version) : name_(std::move(name)), version_(version) {}

// Assigns the root node of the workflows task hierarchy
void WorkflowDefinition::setRoot(std::shared_ptr<WorkflowNode> root)
{
    root_ = std::move(root);
}

// Returns the root node of the workflow hierarchy
std::shared_ptr<WorkflowNode> WorkflowDefinition::getRoot() const
{
    return root_;
}

// Returns the workflow definitions name
const std::string &WorkflowDefinition::getName() const
{
    return name_;
}

// Returns the workflow definitions version
int WorkflowDefinition::getVersion() const
{
    return version_;
}

// Registers a task specification in the workflow definition
void WorkflowDefinition::addTask(std::shared_ptr<Task> t)
{
    if (!t)
        throw std::string("WorkflowDefinition::addTask: task is null");
    tasks_.push_back(std::move(t));
}

// Returns all task specifications registered with the workflow
const std::vector<std::shared_ptr<Task>> &WorkflowDefinition::getTasks() const
{
    return tasks_;
}

// Adds a unique approval handler chain to the workflow definition
void WorkflowDefinition::addChain(ApprovalHandler *h)
{
    if (!h)
        throw std::string("WorkflowDefinition::addChain: chain is null");
    if (std::find(chains_.begin(), chains_.end(), h) == chains_.end())
        chains_.push_back(h);
}

// Returns the approval handler chains used by the workflow
const std::vector<ApprovalHandler *> &WorkflowDefinition::getChains() const
{
    return chains_;
}

// TODO: once WorkflowInstance return an instance built from root_->clone() (Prototype) so every run gets its own copy of the tree
// Creates a runtime workflow instance from this definition
WorkflowInstance *WorkflowDefinition::createInstance() const
{
    throw std::string("WorkflowDefinition::createInstance: WorkflowInstance not implemented yet my guy");
}
