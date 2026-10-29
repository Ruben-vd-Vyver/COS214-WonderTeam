#include "FinalRejectHandler.h"

//The requester's authority is less than the level of the request
bool FinalRejectHandler::canHandle(ApprovalRequest& req) {
    return true;
}

void FinalRejectHandler::decide(ApprovalRequest& req) {
    req.setDecision(false, "FinalRejectHandler: Authority too low for approval");
}