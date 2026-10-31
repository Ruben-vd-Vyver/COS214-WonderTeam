#ifndef AUTHORITYHANDLER_H
#define AUTHORITYHANDLER_H

#include "ApprovalHandler.h"

//ConcreteHandler
class AuthorityHandler : public ApprovalHandler {
    protected:
        bool canHandle(ApprovalRequest& req);
        void decide(ApprovalRequest& req);
    public:
        AuthorityHandler();
};

#endif