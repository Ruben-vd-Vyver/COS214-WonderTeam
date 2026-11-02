#include "LeastWorkloadAssignment.h"

Participant* LeastWorkloadAssignment::select(Task& item, std::vector<Participant*>& pool) {
    Participant* selected = nullptr;
    for (std::size_t i = 0; i < pool.size(); ++i) {
        Participant* candidate = pool[i];
        if (!candidate || !candidate->hasRole(item.getRequiredRole()))
            continue;
        if (!selected || candidate->getWorkload() < selected->getWorkload())
            selected = candidate;
    }
    return selected;
}
