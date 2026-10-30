#include "AuthorityHandler.h"

AuthorityHandler::AuthorityHandler() {}

//Approves if the requester's authority level for an approval is greater than or equal to the level of the request
bool AuthorityHandler::canHandle(ApprovalRequest& req) {
    return (req.getRequester()->getAuthority() >= req.getRequiredLevel());
}

void AuthorityHandler::decide(ApprovalRequest& req) {
    //Maybe print out participant name
    req.setDecision(true, "AuthorityHandler: Authority high enough for approval");
}