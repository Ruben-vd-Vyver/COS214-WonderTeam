#include "WorkAllocator.h"

WorkAllocator::WorkAllocator() {
    strategy = NULL;
}

void WorkAllocator::setStrategy(AssignmentStrategy* strategy) {
    this->strategy = strategy;
}

Participant* WorkAllocator::allocate(Task& item, std::vector<Participant*>& pool) {
    if (strategy == NULL) {
        return NULL;
    }

    Participant* selected = strategy->select(item, pool);

    if (selected != NULL) {
        selected->incrementWorkload();
    }

    return selected;
}