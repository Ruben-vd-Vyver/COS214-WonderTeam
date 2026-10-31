#ifndef COMPLETE_COMMAND_H
#define COMPLETE_COMMAND_H

#include "WorkItemCommand.h"

class CompleteCommand : public WorkItemCommand {
    public:
        explicit CompleteCommand(WorkItem* workItem);
        bool execute() override;
        bool undo() override;
        std::string describe() override;
};

#endif // COMPLETE_COMMAND_H
