#ifndef ASSIGN_COMMAND
#define ASSIGN_COMMAND

#include "WorkItemCommand.h"

class AssignCommand: public WorkItemCommand {
    public:
        AssignCommand(Task* task, Participant* participant);
        void execute();
        void undo();
        std::string describe();
}


#endif //ASSIGN_COMMAND