#ifndef WORKITEMCOMMAND_H
#define WORKITEMCOMMAND_H

#include "Participant.h"
#include "Task.h"

/*
    Interface class for the Command system
*/

class WorkItemCommand {
    protected:
        bool hasExecuted_ = false;
        Task* task_;
        Participant* participant_;
    public: 
        WorkItemCommand(Task* task, Participant* participant);
        virtual void execute() = 0;
        virtual void undo() = 0;
        virtual std::string describe() = 0;
};


#endif //WORKITEMCOMMAND_H