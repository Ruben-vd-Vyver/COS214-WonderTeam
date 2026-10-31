#ifndef ROLEBASEDASSIGNMENT_H
#define ROLEBASEDASSIGNMENT_H

#include "AssignmentStrategy.h"

class RoleBasedAssignment : public AssignmentStrategy {
public:
    Participant* select(Task& item, std::vector<Participant*>& pool);
};

#endif