#ifndef WORKITEMCOMMAND_H
#define WORKITEMCOMMAND_H

#include <memory>
#include "WorkItem.h"
#include "WorkItemMemento.h"

class Participant;

/*
    Interface class for the Command system
*/

class WorkItemCommand {
    protected:
        bool hasExecuted_ = false;
        WorkItem* workItem_;
        Participant* participant_;

        std::unique_ptr<WorkItemMemento> memento_;
        void saveSnapshot();
        bool restoreSnapshot();
    public: 
        WorkItemCommand(WorkItem* workItem, Participant* participant);
        virtual ~WorkItemCommand() = default;
        virtual bool execute() = 0;
        virtual bool undo() = 0;
        virtual std::string describe() = 0;
};


#endif //WORKITEMCOMMAND_H