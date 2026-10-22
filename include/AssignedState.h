#ifndef ASSIGNEDSTATE_H
#define ASSIGNEDSTATE_H

#include "TaskState.h"

class AssignedState : public TaskState
{
public:
    std::string getName() const override;
    bool assign(Task &t, Participant *p) const override;
};

#endif // ASSIGNEDSTATE_H