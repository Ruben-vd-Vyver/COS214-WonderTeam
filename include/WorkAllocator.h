#ifndef WORKALLOCATOR_H
#define WORKALLOCATOR_H

#include "AssignmentStrategy.h"

class WorkAllocator {
private:
    AssignmentStrategy* strategy;

public:
    WorkAllocator();

    void setStrategy(AssignmentStrategy* strategy);

    Participant* allocate(Task& item, std::vector<Participant*>& pool);
};

#endif