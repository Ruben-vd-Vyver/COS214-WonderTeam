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
