#include "PendingState.h"
#include "AvailableState.h"
#include "Task.h"

std::string PendingState::getName() const { return "Pending"; }
void PendingState::activate(Task &t)
{
    markActivated(t);
    changeState(t, new AvailableState());
}

// [Prototype] states hold no data, so a plain copy is a full copy
PendingState *PendingState::clone() const { return new PendingState(*this); }
