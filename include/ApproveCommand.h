#ifndef APPROVE_COMMAND_H
#define APPROVE_COMMAND_H

#include "WorkItemCommand.h"
#include "WorkItemMemento.h"

class ApproveCommand : public WorkItemCommand {
    private:
        WorkItemMemento previousState_;
    public:
        explicit ApproveCommand(WorkItem* workItem);
        bool execute() override;
        bool undo() override;
        std::string describe() override;
};

#endif // APPROVE_COMMAND_H
