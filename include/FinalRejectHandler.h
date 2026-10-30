#ifndef FINALREJECTHANDLER_H
#define FINALREJECTHANDLER_H

#include "ApprovalHandler.h"

//ConcreteHandler
class FinalRejectHandler : public ApprovalHandler {
    protected:
        bool canHandle(ApprovalRequest& req);
        void decide(ApprovalRequest& req);
};

#endif