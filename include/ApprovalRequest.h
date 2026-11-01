#ifndef APPROVALREQUEST_H
#define APPROVALREQUEST_H

#include <string>

class WorkItem;
class Participant;

class ApprovalRequest {
    private:
        WorkItem* item;
        Participant* requester;
        int requiredLevel;
        bool approved;
        bool decided;
        std::string reason;

    public:
        ApprovalRequest(WorkItem* item, Participant* requester,int requiredLevel);
        WorkItem* getWorkItem() const;
        Participant* getRequester() const;
        int getRequiredLevel() const;
        void setDecision(bool approved, const std::string& reason);
        bool isApproved() const;
        bool isDecided() const;
        std::string getReason() const;
};

#endif