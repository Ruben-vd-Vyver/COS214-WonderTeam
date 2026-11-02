#ifndef WORKITEMCOMMAND_H
#define WORKITEMCOMMAND_H

#include <memory>
#include "WorkItem.h"
#include "WorkItemMemento.h"

/**
 * @brief Represents the participant abstraction in the TIVIDY workflow system.
 */
class Participant;

/*
    Interface class for the Command system
*/

/**
 * @brief Represents the work item command abstraction in the TIVIDY workflow system.
 */
class WorkItemCommand {
    protected:
        bool hasExecuted_ = false;
        WorkItem* workItem_;
        Participant* participant_;

        std::unique_ptr<WorkItemMemento> memento_;
        /**
         * @brief Performs the save snapshot operation.
         */
        void saveSnapshot();
        /**
         * @brief Performs the restore snapshot operation.
         * @return Result produced by the operation.
         */
        bool restoreSnapshot();
    public: 
        /**
         * @brief Constructs an object of this type.
         * @param workItem Value or object supplied by the caller.
         * @param participant Value or object supplied by the caller.
         */
        WorkItemCommand(WorkItem* workItem, Participant* participant);
        /**
         * @brief Destroys this object and releases owned resources.
         */
        virtual ~WorkItemCommand() = default;
        /**
         * @brief Performs the execute operation.
         * @return Result produced by the operation.
         */
        virtual bool execute() = 0;
        /**
         * @brief Performs the undo operation.
         * @return Result produced by the operation.
         */
        virtual bool undo() = 0;
        /**
         * @brief Performs the describe operation.
         * @return Result produced by the operation.
         */
        virtual std::string describe() = 0;
};


#endif //WORKITEMCOMMAND_H