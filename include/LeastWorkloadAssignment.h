#ifndef LEASTWORKLOADASSIGNMENT_H
#define LEASTWORKLOADASSIGNMENT_H

#include "AssignmentStrategy.h"

/**
 * @brief Represents the least workload assignment abstraction in the TIVIDY workflow system.
 */
class LeastWorkloadAssignment : public AssignmentStrategy {
public:
    /**
     * @brief Performs the select operation.
     * @param item Value or object supplied by the caller.
     * @param pool Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    Participant* select(Task& item,std::vector<Participant*>& pool);
};

#endif
