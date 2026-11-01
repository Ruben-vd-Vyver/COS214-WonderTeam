#ifndef APPROVALSERVICE_H
#define APPROVALSERVICE_H

#include <string>
#include <vector>

#include "WorkflowObserver.h"

//ConcreteObserver
class ApprovalService : public WorkflowObserver {
    private:
        std::vector<std::string> approvalEvents;

    public:
        void onEvent(const WorkflowEvent& event);
        void runApproval(const WorkflowEvent& event);
        const std::vector<std::string>& getApprovalEvents() const;
        void printApprovalEvents() const;
};

#endif