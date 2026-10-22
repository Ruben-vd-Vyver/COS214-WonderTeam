#include "PendingState.h"
#include "AvailableState.h"
#include "Task.h"

std::string PendingState::getName() const { return "Pending"; }
void PendingState::activate(Task &t)
{
    markActivated(t);
    changeState(t, new AvailableState());
}