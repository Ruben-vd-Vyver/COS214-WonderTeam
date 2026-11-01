#ifndef LEASTWORKLOADASSIGNMENT_H
#define LEASTWORKLOADASSIGNMENT_H

#include "AssignmentStrategy.h"

class LeastWorkloadAssignment : public AssignmentStrategy {
public:
    Participant* select(Task& item,std::vector<Participant*>& pool);
};

#endif