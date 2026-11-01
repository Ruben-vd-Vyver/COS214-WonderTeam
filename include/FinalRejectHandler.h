#ifndef FINALREJECTHANDLER_H
#define FINALREJECTHANDLER_H

#include "ApprovalHandler.h"

//ConcreteHandler
/**
 * @brief Represents the final reject handler abstraction in the TIVIDY workflow system.
 */
class FinalRejectHandler : public ApprovalHandler {
    protected:
        /**
         * @brief Reports whether the requested condition is satisfied.
         * @param req Value or object supplied by the caller.
         * @return Result produced by the operation.
         */
        bool canHandle(ApprovalRequest& req);
        /**
         * @brief Performs the decide operation.
         * @param req Value or object supplied by the caller.
         */
        void decide(ApprovalRequest& req);
};

#endif
