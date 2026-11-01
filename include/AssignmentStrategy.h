#ifndef ASSIGNMENTSTRATEGY_H
#define ASSIGNMENTSTRATEGY_H

#include <vector>
#include "Participant.h"
#include "Task.h"

/**
 * @brief Represents the assignment strategy abstraction in the TIVIDY workflow system.
 */
class AssignmentStrategy {
public:
    /**
     * @brief Performs the select operation.
     * @param item Value or object supplied by the caller.
     * @param pool Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    virtual Participant* select(Task& item, std::vector<Participant*>& pool) = 0;
    /**
     * @brief Destroys this object and releases owned resources.
     */
    virtual ~AssignmentStrategy();
};

#endif
