#ifndef WORKFLOWINSTANCE_H
#define WORKFLOWINSTANCE_H

#include <ctime>
#include <map>
#include <string>

#include "WorkflowSubject.h"

class WorkItem;

//ConcreteSubject
class WorkflowInstance : public WorkflowSubject {
    private:
        std::string instanceId;
        std::map<std::string, std::string> data;

        void publishEvent(const std::string& type, WorkItem* item, const std::string& detail,
            std::time_t deadline = 0, bool hasDeadline = false);

    public:
        explicit WorkflowInstance( const std::string& instanceId);
        const std::string& getInstanceId() const;
        void start();
        void setData(const std::string& key, const std::string& value);
        bool getData(const std::string& key, std::string& value) const;
        void createWorkItem(WorkItem* item);
        void requestApproval(WorkItem* item);
        void approve(WorkItem* item);
        void reject(WorkItem* item);
        void completeWorkItem(WorkItem* item);
        void setDeadline(WorkItem* item, std::time_t deadline);
};

#endif