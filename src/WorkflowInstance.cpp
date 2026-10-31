#include "WorkflowInstance.h"
#include "WorkflowEvent.h"
// #include "WorkItem.h"

#include <sstream>

WorkflowInstance::WorkflowInstance(const std::string& id) : instanceId(id) {}

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