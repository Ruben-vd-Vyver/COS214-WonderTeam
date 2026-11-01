#include "WorkflowInstance.h"
#include "WorkflowEvent.h"
#include "WorkflowDefinition.h"
#include "WorkflowNode.h"
#include "WorkItem.h"
#include "WorkItemCommand.h"

#include <sstream>
#include <stdexcept>

WorkflowInstance::WorkflowInstance(const WorkflowDefinition &def, WorkflowNode *root, const std::string &id)
    : instanceId(id), root_(root)
{
    if (!root_)
        throw std::invalid_argument("WorkflowInstance needs a root (a clone of the definition's tree)");
    data["definition"] = def.getName();
    root_->bind(this); // every work item in this copy reports its events to this instance
}

WorkflowInstance::~WorkflowInstance() {}

bool WorkflowInstance::perform(WorkItemCommand *cmd)
{
    std::unique_ptr<WorkItemCommand> owned(cmd); // deleted here unless history keeps it
    if (!owned)
        return false;
    if (!history_.run(owned.get()))
        return false;
    owned.release(); // history now owns it
    return true;
}

bool WorkflowInstance::undoLast() { return history_.undo(); }

WorkflowNode *WorkflowInstance::getRoot() const { return root_.get(); }

WorkItem *WorkflowInstance::findItem(const std::string &id) const { return root_->find(id); }

bool WorkflowInstance::isComplete() const { return root_->isComplete(); }

WorkAllocator &WorkflowInstance::getAllocator() { return allocator_; }

std::string WorkflowInstance::getData(const std::string &key) const
{
    std::string value;
    getData(key, value);
    return value;
}

const std::string& WorkflowInstance::getInstanceId() const {
    return instanceId;
}

void WorkflowInstance::publishEvent(const std::string& type, WorkItem* item, const std::string& detail, std::time_t deadline, bool hasDeadline) {
    notify(WorkflowEvent(type, item, this, detail, deadline,hasDeadline));
}

void WorkflowInstance::start() {
    publishEvent("WorkflowStarted", NULL,instanceId);
}

void WorkflowInstance::setData(const std::string& key,const std::string& value) {
    data[key] = value;
    publishEvent("WorkflowDataUpdated", NULL, key);
}

bool WorkflowInstance::getData(const std::string& key, std::string& value) const {
    std::map<std::string, std::string>::const_iterator it = data.find(key);

    if (it == data.end()) {
        return false;
    }
    value = it->second;
    return true;
}

void WorkflowInstance::createWorkItem(WorkItem* item) {
    if (item == NULL) {
        return;
    }
    publishEvent("WorkItemCreated", item, "Work item created");
}

void WorkflowInstance::requestApproval( WorkItem* item) {
    if (item == NULL) {
        return;
    }

    publishEvent("ApprovalRequested",  item,"Approval requested");
}

//Chain of resp??
void WorkflowInstance::approve(WorkItem* item) {
    if (item == NULL) {
        return;
    }
    publishEvent("ApprovalApproved", item, "Approval granted");
}

void WorkflowInstance::reject(WorkItem* item) {
    if (item == NULL) {
        return;
    }
    publishEvent("ApprovalRejected", item, "Approval rejected");
}

void WorkflowInstance::completeWorkItem(WorkItem* item) {
    if (item == NULL) {
        return;
    }
    publishEvent("WorkItemCompleted", item, "Work item completed");
}

void WorkflowInstance::setDeadline(WorkItem* item, std::time_t deadline) {
    if (item == NULL) {
        return;
    }
    publishEvent("DeadlineSet", item, "Deadline assigned", deadline, true);
}