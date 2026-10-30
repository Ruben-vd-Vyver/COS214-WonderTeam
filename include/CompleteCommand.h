#ifndef COMPLETE_COMMAND_H
#define COMPLETE_COMMAND_H

#include "WorkItemCommand.h"
#include "WorkItemMemento.h"

class CompleteCommand : public WorkItemCommand {
    private:
        WorkItemMemento previousState_;
    public:
        explicit CompleteCommand(WorkItem* workItem);
        bool execute() override;
        bool undo() override;
        std::string describe() override;
};

#endif // COMPLETE_COMMAND_H
