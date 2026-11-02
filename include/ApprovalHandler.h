#ifndef APPROVALHANDLER_H
#define APPROVALHANDLER_H

#include "ApprovalRequest.h"

//Handler
/**
 * @brief Represents the approval handler abstraction in the TIVIDY workflow system.
 */
class ApprovalHandler {
    protected:
        ApprovalHandler* next;
        /**
         * @brief Reports whether the requested condition is satisfied.
         * @param req Value or object supplied by the caller.
         * @return Result produced by the operation.
         */
        virtual bool canHandle(ApprovalRequest& req) = 0;
        /**
         * @brief Performs the decide operation.
         * @param req Value or object supplied by the caller.
         */
        virtual void decide(ApprovalRequest& req) = 0;

    public:
        /**
         * @brief Constructs an object of this type.
         */
        ApprovalHandler();
        /**
         * @brief Destroys this object and releases owned resources.
         */
        virtual ~ApprovalHandler();
        /**
         * @brief Sets the next.
         * @param handler Value or object supplied by the caller.
         * @return Result produced by the operation.
         */
        ApprovalHandler* setNext(ApprovalHandler* handler);
        /**
         * @brief Performs the handle operation.
         * @param req Value or object supplied by the caller.
         */
        virtual void handle(ApprovalRequest& req);
};

#endif