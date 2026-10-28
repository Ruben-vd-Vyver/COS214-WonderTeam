#include "AuthorityHandler.h"

AuthorityHandler::AuthorityHandler(int window): responseWindow(window) {}

bool AuthorityHandler::canHandle(ApprovalRequest& req) {
    return req.getRequester().getAuthority() > req.getRequiredLevel();
}

void AuthorityHandler::decide(ApprovalRequest& req) {
    //Maybe print out participant name
    req.setDecision(false, "AuthorityHandler: manual review required");
}