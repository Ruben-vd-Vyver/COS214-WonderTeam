#include "LeastWorkloadAssignment.h"

Participant* LeastWorkloadAssignment::select(Task& item, std::vector<Participant*>& pool) {
    if (pool.empty()) { //checks empty
        return NULL;
    }

    Participant* selected = pool[0];

    for (int i = 1; i < pool.size(); i++) {
        //lower workload updates selescted
        if (pool[i]->getWorkload() <
            selected->getWorkload()) {

            selected = pool[i];
        }
    }

    return selected;
}