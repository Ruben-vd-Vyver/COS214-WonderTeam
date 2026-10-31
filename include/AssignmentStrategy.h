#ifndef ASSIGNMENTSTRATEGY_H
#define ASSIGNMENTSTRATEGY_H

#include <vector>
#include "Participant.h"
#include "Task.h"

class AssignmentStrategy {
public:
    virtual Participant* select(Task& item, std::vector<Participant*>& pool) = 0;
    virtual ~AssignmentStrategy();
};

#endif