#include "AutoApprovalHandler.h"

AutoApprovalHandler::AutoApprovalHandler(int maxLevel): maxAutoLevel(maxLevel) {}

bool AutoApprovalHandler::canHandle(ApprovalRequest& req) {
    return req.getRequiredLevel() <= maxAutoLevel;
}

void AutoApprovalHandler::decide(ApprovalRequest& req) {
    req.setDecision(true, "AutoApprovalHandler");
}