#include "FinalRejectHandler.h"

bool FinalRejectHandler::canHandle(ApprovalRequest& req) {
    return true;
}

void FinalRejectHandler::decide(ApprovalRequest& req) {
    req.setDecision(false, "FinalRejectHandler");
}