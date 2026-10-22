#include "AssignedState.h"
#include "InProgressState.h"
#include "EscalatedState.h"
#include "Task.h"

std::string AssignedState::getName() const { return "Assigned"; }
bool AssignedState::assign(Task &t, Participant *p)
{
    if (!p)
        return false;
    setAssignee(t, p);
    return true;
}
bool AssignedState::start(Task &t)
{
    changeState(t, new InProgressState());
    return true;
}
bool AssignedState::escalate(Task &t)
{
    changeState(t, new EscalatedState());
    return true;
}