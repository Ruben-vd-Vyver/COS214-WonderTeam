#include "AvailableState.h"
#include "AssignedState.h"
#include "Task.h"

std::string AvailableState::getName() const { return "Available"; }
bool AvailableState::assign(Task &t, Participant *p)
{
    if (!p)
        return false;
    setAssignee(t, p);
    changeState(t, new AssignedState());
    return true;
}