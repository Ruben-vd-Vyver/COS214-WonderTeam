#ifndef ASSIGNEDSTATE_H
#define ASSIGNEDSTATE_H

#include "TaskState.h"

class AssignedState : public TaskState
{
public:
    std::string getName() const override;
    bool assign(Task &t, Participant *p) override; // reassign, stays Assigned
    bool start(Task &t) override;                  // -> InProgress
    bool escalate(Task &t) override;               // -> Escalated
};

#endif
