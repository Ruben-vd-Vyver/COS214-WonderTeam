#ifndef REJECT_COMMAND_H
#define REJECT_COMMAND_H

#include "WorkItemCommand.h"

class RejectCommand : public WorkItemCommand {
    private:
        std::string reason_;
    public:
        RejectCommand(WorkItem* workItem, std::string reason);
        bool execute() override;
        bool undo() override;
        std::string describe() override;
};

#endif // REJECT_COMMAND_H
