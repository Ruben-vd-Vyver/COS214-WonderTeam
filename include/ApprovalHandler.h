#ifndef APPROVALHANDLER_H
#define APPROVALHANDLER_H

#include "ApprovalRequest.h"

//Handler
class ApprovalHandler {
    protected:
        ApprovalHandler* next;
        virtual bool canHandle(ApprovalRequest& req) = 0;
        virtual void decide(ApprovalRequest& req) = 0;

    public:
        ApprovalHandler();
        virtual ~ApprovalHandler();
        ApprovalHandler* setNext(ApprovalHandler* handler);
        virtual void handle(ApprovalRequest& req);
};

#endif