#ifndef COMMAND_HISTORY
#define COMMAND_HISTORY

#include <iostream>
#include <string>
#include <vector>
#include "WorkItemCommand.h"


class CommandHistory {
    private:
        std::vector<WorkItemCommand*> history_;
        std::vector<WorkItemCommand*> future_;
        const int hist_limit = 15;
        const int fut_limit = 5;
    public:
        CommandHistory() = default;
        ~CommandHistory();
        CommandHistory(const CommandHistory&) = delete;
        CommandHistory& operator=(const CommandHistory&) = delete;
        bool run(WorkItemCommand*);
        bool undo();
        bool redo();
        std::vector<std::string> log();
};



#endif // COMMAND_HISTORY