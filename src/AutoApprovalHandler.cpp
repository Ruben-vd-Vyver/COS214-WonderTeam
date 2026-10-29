#include "AutoApprovalHandler.h"

AutoApprovalHandler::AutoApprovalHandler(int maxLevel): maxAutoLevel(maxLevel) {}

//Approves if the level of the request is less than or equal to our set level for auto approvals
bool AutoApprovalHandler::canHandle(ApprovalRequest& req) {
    return (req.getRequiredLevel() <= maxAutoLevel);
}

void AutoApprovalHandler::decide(ApprovalRequest& req) {
    req.setDecision(true, "AutoApprovalHandler");
}