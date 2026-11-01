#ifndef APPROVALSERVICE_H
#define APPROVALSERVICE_H

#include <string>
#include <vector>

#include "WorkflowObserver.h"

//ConcreteObserver
/**
 * @brief Represents the approval service abstraction in the TIVIDY workflow system.
 */
class ApprovalService : public WorkflowObserver {
    private:
        std::vector<std::string> approvalEvents;

    public:
        /**
         * @brief Performs the on event operation.
         * @param event Value or object supplied by the caller.
         */
        void onEvent(const WorkflowEvent& event);
        /**
         * @brief Performs the run approval operation.
         * @param event Value or object supplied by the caller.
         */
        void runApproval(const WorkflowEvent& event);
        /**
         * @brief Returns the approval events.
         * @return Result produced by the operation.
         */
        const std::vector<std::string>& getApprovalEvents() const;
        /**
         * @brief Performs the print approval events operation.
         */
        void printApprovalEvents() const;
};

#endif