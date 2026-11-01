#ifndef RULEBASEDASSIGNMENT_H
#define RULEBASEDASSIGNMENT_H

#include "AssignmentStrategy.h"

class RuleBasedAssignment : public AssignmentStrategy {
public:
    Participant* select(Task& item, std::vector<Participant*>& pool);
};

#endif