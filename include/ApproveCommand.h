#ifndef APPROVE_COMMAND_H
#define APPROVE_COMMAND_H

#include "WorkItemCommand.h"

class ApproveCommand : public WorkItemCommand {
    public:
        explicit ApproveCommand(WorkItem* workItem);
        bool execute() override;
        bool undo() override;
        std::string describe() override;
};

#endif // APPROVE_COMMAND_H
