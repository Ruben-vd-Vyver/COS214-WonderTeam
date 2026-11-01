#ifndef START_COMMAND_H
#define START_COMMAND_H

#include "WorkItemCommand.h"

/**
 * @brief Represents the start command abstraction in the TIVIDY workflow system.
 */
class StartCommand : public WorkItemCommand {
    public:
        /**
         * @brief Constructs an object of this type.
         * @param workItem Value or object supplied by the caller.
         */
        explicit StartCommand(WorkItem* workItem);
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

#endif // START_COMMAND_H
