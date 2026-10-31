#include "WorkflowInstance.h"
#include "WorkflowEvent.h"
#include "WorkflowDefinition.h"
#include "WorkflowNode.h"
#include "WorkItem.h"
#include "WorkItemCommand.h"
#include <stdexcept>

WorkflowInstance::WorkflowInstance(const WorkflowDefinition &def, WorkflowNode *root, const std::string &id) : instanceId(id), root_(root)
{
    if (!root_)
        throw std::invalid_argument("WorkflowInstance needs a root node");
    data["workflow"] = def.getName();
    data["version"] = std::to_string(def.getVersion());
    root_->bind(this);
}

WorkflowInstance::WorkflowInstance(const std::string &id) : instanceId(id) {}

WorkflowInstance::~WorkflowInstance() {}

const std::string &WorkflowInstance::getInstanceId() const
{
    return instanceId;
}

void WorkflowInstance::publishEvent(const std::string &type, WorkItem *item, const std::string &detail, std::time_t deadline, bool hasDeadline)
{
    notify(WorkflowEvent(type, item, this, detail, deadline, hasDeadline));
}

void WorkflowInstance::start()
{
    publishEvent("WorkflowStarted", nullptr, instanceId);
    if (root_)
        root_->activate();
}

void WorkflowInstance::perform(WorkItemCommand *cmd)
{
    if (!cmd)
        throw std::invalid_argument("WorkflowInstance::perform needs a command");
    history_.run(cmd);
}

bool WorkflowInstance::undoLast()
{
    return history_.undoLast();
}

WorkflowNode *WorkflowInstance::getRoot() const
{
    return root_.get();
}

WorkItem *WorkflowInstance::findItem(const std::string &id) const
{
    return root_ ? root_->find(id) : nullptr;
}

bool WorkflowInstance::isComplete() const
{
    return root_ && root_->isComplete();
}

WorkAllocator &WorkflowInstance::getAllocator()
{
    return allocator_;
}

void WorkflowInstance::setData(const std::string &key, const std::string &value)
{
    data[key] = value;
    publishEvent("WorkflowDataUpdated", nullptr, key);
}

bool WorkflowInstance::getData(const std::string &key, std::string &value) const
{
    std::map<std::string, std::string>::const_iterator it = data.find(key);
    if (it == data.end())
        return false;
    value = it->second;
    return true;
}

std::string WorkflowInstance::getData(const std::string &key) const
{
    std::map<std::string, std::string>::const_iterator it = data.find(key);
    return it == data.end() ? std::string() : it->second;
}

void WorkflowInstance::createWorkItem(WorkItem *item)
{
    if (item)
        publishEvent("WorkItemCreated", item, "Work item created");
}

void WorkflowInstance::requestApproval(WorkItem *item)
{
    if (item)
        publishEvent("ApprovalRequested", item, "Approval requested");
}

void WorkflowInstance::approve(WorkItem *item)
{
    if (item)
        publishEvent("ApprovalApproved", item, "Approval granted");
}

void WorkflowInstance::reject(WorkItem *item)
{
    if (item)
        publishEvent("ApprovalRejected", item, "Approval rejected");
}

void WorkflowInstance::completeWorkItem(WorkItem *item)
{
    if (item)
        publishEvent("WorkItemCompleted", item, "Work item completed");
}

void WorkflowInstance::setDeadline(WorkItem *item, std::time_t deadline)
{
    if (item)
        publishEvent("DeadlineSet", item, "Deadline assigned", deadline, true);
}
