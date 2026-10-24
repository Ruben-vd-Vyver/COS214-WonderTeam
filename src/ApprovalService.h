#ifndef APPROVALSERVICE_H
#define APRROVALSERVICE_H

#include <string>
#include <vector>
#include "WorkflowObserver.h"

//ConcreteObserver
class ApprovalService : public WorkflowObserver {
    private:
        std::vector<std::string> approvalEvents;
        bool isApprovalEvent(const std::string& eventType) const;
    public:
        void onEvent(const WorkflowEvent& event);
        const std::vector<std::string>& getApprovalEvents() const;
        void printApprovalEvents() const;
    };


#endif