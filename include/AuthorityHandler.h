#ifndef AUTHORITYHANDLER_H
#define AUTHORITYHANDLER_H

#include "ApprovalHandler.h"

//ConcreteHandler
class AuthorityHandler : public ApprovalHandler {
    private:
        int responseWindow;
    protected:
        bool canHandle(ApprovalRequest& req);
        void decide(ApprovalRequest& req);
    public:
        AuthorityHandler(int window);
};

#endif