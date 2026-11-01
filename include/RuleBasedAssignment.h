#ifndef RULEBASEDASSIGNMENT_H
#define RULEBASEDASSIGNMENT_H

#include "AssignmentStrategy.h"

/**
 * @brief Represents the rule based assignment abstraction in the TIVIDY workflow system.
 */
class RuleBasedAssignment : public AssignmentStrategy {
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