#ifndef COMMAND_HISTORY
#define COMMAND_HISTORY

#include <iostream>
#include <string>
#include <vector>
#include "WorkItemCommand.h"


/**
 * @brief Represents the command history abstraction in the TIVIDY workflow system.
 */
class CommandHistory {
    private:
        std::vector<WorkItemCommand*> history_;
        std::vector<WorkItemCommand*> future_;
        const int hist_limit = 15;
        const int fut_limit = 5;
    public:
        /**
         * @brief Constructs an object of this type.
         */
        CommandHistory() = default;
        /**
         * @brief Destroys this object and releases owned resources.
         */
        ~CommandHistory();
        /**
         * @brief Constructs an object of this type.
         */
        CommandHistory(const CommandHistory&) = delete;
        CommandHistory& operator=(const CommandHistory&) = delete;
        /**
         * @brief Performs the run operation.
         * @return Result produced by the operation.
         */
        bool run(WorkItemCommand*);
        /**
         * @brief Performs the undo operation.
         * @return Result produced by the operation.
         */
        bool undo();
        /**
         * @brief Performs the redo operation.
         * @return Result produced by the operation.
         */
        bool redo();
        /**
         * @brief Performs the log operation.
         * @return Result produced by the operation.
         */
        std::vector<std::string> log();
};



#endif // COMMAND_HISTORY