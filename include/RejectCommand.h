#ifndef REJECT_COMMAND_H
#define REJECT_COMMAND_H

#include "WorkItemCommand.h"

/**
 * @brief Represents the reject command abstraction in the TIVIDY workflow system.
 */
class RejectCommand : public WorkItemCommand {
    private:
        std::string reason_;
    public:
        /**
         * @brief Constructs an object of this type.
         * @param workItem Value or object supplied by the caller.
         * @param reason Value or object supplied by the caller.
         */
        RejectCommand(WorkItem* workItem, std::string reason);
        /**
         * @brief Performs the execute operation.
         * @return Result produced by the operation.
         */
        bool execute() override;
        /**
         * @brief Performs the undo operation.
         * @return Result produced by the operation.
         */
        bool undo() override;
        /**
         * @brief Performs the describe operation.
         * @return Result produced by the operation.
         */
        std::string describe() override;
};

#endif // REJECT_COMMAND_H
