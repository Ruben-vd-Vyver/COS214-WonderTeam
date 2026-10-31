#ifndef AUTOAPPROVALHANDLER_H
#define AUTOAPPROVALHANDLER_H

#include "ApprovalHandler.h"

//ConcreteHandler
class AutoApprovalHandler : public ApprovalHandler {
    private:
        int maxAutoLevel;
    protected:
        bool canHandle(ApprovalRequest& req);
        void decide(ApprovalRequest& req);
    public:
        AutoApprovalHandler(int maxLevel);
};

#endif