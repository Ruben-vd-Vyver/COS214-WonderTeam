#include "PendingState.h"
#include "AvailableState.h"
#include "BasicWorkItem.h"

std::string PendingState::getName() const { return "Pending"; }
WorkItemState *PendingState::clone() const { return new PendingState(); }
void PendingState::activate(BasicWorkItem &item)
{
    item.setState(new AvailableState());
}
