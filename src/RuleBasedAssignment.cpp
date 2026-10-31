#include "RuleBasedAssignment.h"

//Rule: Participant must have the required role and a workload below 5
Participant* RuleBasedAssignment::select(Task& item, std::vector<Participant*>& pool) {
    for (int i = 0; i < pool.size(); i++) {

        if (pool[i]->hasRole(item.getRequiredRole()) &&
            pool[i]->getWorkload() < 5) {

            return pool[i];
        }
    }

    return NULL;
}