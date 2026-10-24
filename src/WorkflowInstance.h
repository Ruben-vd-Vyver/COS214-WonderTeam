#ifndef WORKFLOWINSTANCE_H
#define WORKFLOWINSTANCE_H

#include <map>
#include <string>
#include "WorkflowSubject.h"

class WorkflowInstance : public WorkflowSubject {
    private:
        std::string instanceId;
        std::map<std::string, std::string> data;
    public:
        WorkflowInstance(const std::string& instanceId);
        const std::string& getInstanceId() const;
        void setData(const std::string& key, const std::string& value);
        bool getData(const std::string& key, std::string& value) const;
        void createWorkItem(const std::string& workItemId);
        void requestApproval(const std::string& workItemId);
        void approve(const std::string& workItemId);
        void reject(const std::string& workItemId);
        void completeWorkItem(const std::string& workItemId);
};

#endif