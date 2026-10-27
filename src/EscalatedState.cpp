#include "EscalatedState.h"
#include "AssignedState.h"
#include "Task.h"

std::string EscalatedState::getName() const { return "Escalated"; }
bool EscalatedState::assign(Task &t, Participant *p)
{
    if (!p)
        return false;
    setAssignee(t, p);
    changeState(t, new AssignedState());
    return true;
}

// [Prototype] states hold no data, so a plain copy is a full copy
EscalatedState *EscalatedState::clone() const { return new EscalatedState(*this); }
