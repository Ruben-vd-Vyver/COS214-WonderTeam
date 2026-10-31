#include "RoleBasedAssignment.h"

//returns first participant who matches, else null
Participant* RoleBasedAssignment::select(Task& item, std::vector<Participant*>& pool) {
    for (int i = 0; i < pool.size(); i++) {

        if (pool[i]->hasRole(item.getRequiredRole())) {
            return pool[i];
        }
    }

    return NULL;
}