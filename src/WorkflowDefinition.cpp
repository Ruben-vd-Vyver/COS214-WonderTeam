#include "WorkflowDefinition.h"
#include <algorithm> //used for find in chains_
#include "Task.h"
#include "WorkflowInstance.h"
#include "WorkflowNode.h"

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

// Creates a runtime instance holding its own deep copy of the tree, so runs never share state
WorkflowInstance *WorkflowDefinition::createInstance(const std::string &id) const
{
    if (!root_)
        throw std::string("WorkflowDefinition::createInstance: definition has no root");
    static int runCount = 0;
    std::string runId = id.empty() ? name_ + "-v" + std::to_string(version_) + "-" + std::to_string(++runCount) : id;
    return new WorkflowInstance(*this, root_->clone(), runId); // the instance owns the copy
}
