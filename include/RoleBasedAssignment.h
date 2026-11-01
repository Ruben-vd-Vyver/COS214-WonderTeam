#ifndef ROLEBASEDASSIGNMENT_H
#define ROLEBASEDASSIGNMENT_H

#include "AssignmentStrategy.h"

/**
 * @brief Represents the role based assignment abstraction in the TIVIDY workflow system.
 */
class RoleBasedAssignment : public AssignmentStrategy {
public:
    /**
     * @brief Performs the select operation.
     * @param item Value or object supplied by the caller.
     * @param pool Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    Participant* select(Task& item, std::vector<Participant*>& pool);
};

#endif
