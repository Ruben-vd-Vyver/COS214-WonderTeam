#include "ApprovalRequest.h"

ApprovalRequest::ApprovalRequest(WorkItem* item, Participant* requester, int requiredLevel)
    : item(item), requester(requester), requiredLevel(requiredLevel),
      approved(false), decided(false), reason("") {}

WorkItem* ApprovalRequest::getWorkItem() const {
    return item;
}

Participant* ApprovalRequest::getRequester() const {
    return requester;
}

int ApprovalRequest::getRequiredLevel() const {
    return requiredLevel;
}

void ApprovalRequest::setDecision(bool approved, const std::string& reason) {
    this->approved = approved;
    this->reason = reason;
    decided = true;
}

bool ApprovalRequest::isApproved() const {
    return approved;
}

bool ApprovalRequest::isDecided() const {
    return decided;
}

std::string ApprovalRequest::getReason() const {
    return reason;
}

