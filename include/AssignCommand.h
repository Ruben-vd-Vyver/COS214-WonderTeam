#ifndef ASSIGN_COMMAND
#define ASSIGN_COMMAND

#include "WorkItemCommand.h"

class AssignCommand: public WorkItemCommand {
    public:
        AssignCommand(WorkItem* workItem, Participant* participant);
        bool execute();
        bool undo();
        std::string describe();
};


#endif //ASSIGN_COMMAND