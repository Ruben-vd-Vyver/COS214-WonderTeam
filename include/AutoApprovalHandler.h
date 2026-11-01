#ifndef AUTOAPPROVALHANDLER_H
#define AUTOAPPROVALHANDLER_H

#include "ApprovalHandler.h"

//ConcreteHandler
/**
 * @brief Represents the auto approval handler abstraction in the TIVIDY workflow system.
 */
class AutoApprovalHandler : public ApprovalHandler {
    private:
        int maxAutoLevel;
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
    public:
        /**
         * @brief Constructs an object of this type.
         * @param maxLevel Value or object supplied by the caller.
         */
        AutoApprovalHandler(int maxLevel);
};

#endif
