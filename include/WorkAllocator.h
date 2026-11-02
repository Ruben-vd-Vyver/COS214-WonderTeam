#ifndef WORKALLOCATOR_H
#define WORKALLOCATOR_H

#include "AssignmentStrategy.h"

/**
 * @brief Represents the work allocator abstraction in the TIVIDY workflow system.
 */
class WorkAllocator {
private:
    AssignmentStrategy* strategy;

public:
    /**
     * @brief Constructs an object of this type.
     */
    WorkAllocator();

    /**
     * @brief Sets the strategy.
     * @param strategy Value or object supplied by the caller.
     */
    void setStrategy(AssignmentStrategy* strategy);

    /**
     * @brief Performs the allocate operation.
     * @param item Value or object supplied by the caller.
     * @param pool Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    Participant* allocate(Task& item, std::vector<Participant*>& pool);
};

#endif