#ifndef ASSIGNEDSTATE_H
#define ASSIGNEDSTATE_H

#include "TaskState.h"

class AssignedState : public TaskState
{
public:
    std::string getName() const override;
    bool assign(Task &t, Participant *p) override; 
    bool start(Task &t) override;                 
    bool escalate(Task &t) override;              

    // [Prototype]
    AssignedState *clone() const override;
};

#endif
