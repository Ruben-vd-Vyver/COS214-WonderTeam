#ifndef ASSIGN_COMMAND
#define ASSIGN_COMMAND

#include "WorkItemCommand.h"

/**
 * @brief Represents the assign command abstraction in the TIVIDY workflow system.
 */
class AssignCommand: public WorkItemCommand {
    public:
        /**
         * @brief Constructs an object of this type.
         * @param workItem Value or object supplied by the caller.
         * @param participant Value or object supplied by the caller.
         */
        AssignCommand(WorkItem* workItem, Participant* participant);
        /**
         * @brief Performs the execute operation.
         * @return Result produced by the operation.
         */
        bool execute();
        /**
         * @brief Performs the undo operation.
         * @return Result produced by the operation.
         */
        bool undo();
        /**
         * @brief Performs the describe operation.
         * @return Result produced by the operation.
         */
        std::string describe();
};


#endif //ASSIGN_COMMAND