
#include "ApprovalHandler.h"

ApprovalHandler::ApprovalHandler() : next(0) {}

ApprovalHandler::~ApprovalHandler() {}

ApprovalHandler* ApprovalHandler::setNext(ApprovalHandler* handler) {
    next = handler;
    return handler;
}

void ApprovalHandler::handle(ApprovalRequest& req) {
    if (canHandle(req)) {
        decide(req);
    } else if (next != 0) {
        next->handle(req);
    }
}