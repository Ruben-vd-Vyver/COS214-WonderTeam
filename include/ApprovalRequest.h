#ifndef APPROVALREQUEST_H
#define APPROVALREQUEST_H

#include <string>

/**
 * @brief Represents the work item abstraction in the TIVIDY workflow system.
 */
class WorkItem;
/**
 * @brief Represents the participant abstraction in the TIVIDY workflow system.
 */
class Participant;

/**
 * @brief Represents the approval request abstraction in the TIVIDY workflow system.
 */
class ApprovalRequest {
    private:
        WorkItem* item;
        Participant* requester;
        int requiredLevel;
        bool approved;
        bool decided;
        std::string reason;

    public:
        /**
         * @brief Constructs an object of this type.
         * @param item Value or object supplied by the caller.
         * @param requester Value or object supplied by the caller.
         * @param requiredLevel Value or object supplied by the caller.
         */
        ApprovalRequest(WorkItem* item, Participant* requester,int requiredLevel);
        /**
         * @brief Returns the work item.
         * @return Result produced by the operation.
         */
        WorkItem* getWorkItem() const;
        /**
         * @brief Returns the requester.
         * @return Result produced by the operation.
         */
        Participant* getRequester() const;
        /**
         * @brief Returns the required level.
         * @return Result produced by the operation.
         */
        int getRequiredLevel() const;
        /**
         * @brief Sets the decision.
         * @param approved Value or object supplied by the caller.
         * @param reason Value or object supplied by the caller.
         */
        void setDecision(bool approved, const std::string& reason);
        /**
         * @brief Reports whether the requested condition is satisfied.
         * @return Result produced by the operation.
         */
        bool isApproved() const;
        /**
         * @brief Reports whether the requested condition is satisfied.
         * @return Result produced by the operation.
         */
        bool isDecided() const;
        /**
         * @brief Returns the reason.
         * @return Result produced by the operation.
         */
        std::string getReason() const;
};

#endif
