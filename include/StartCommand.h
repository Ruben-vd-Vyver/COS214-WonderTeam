#ifndef START_COMMAND_H
#define START_COMMAND_H

#include "WorkItemCommand.h"
#include "WorkItemMemento.h"

class StartCommand : public WorkItemCommand {
    private:
        WorkItemMemento previousState_;
    public:
        explicit StartCommand(WorkItem* workItem);
        bool execute() override;
        bool undo() override;
        std::string describe() override;
};

#endif // START_COMMAND_H
