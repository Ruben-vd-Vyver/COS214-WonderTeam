#include "AvailableState.h"
#include "AssignedState.h"
#include "EscalatedState.h"
#include "BasicWorkItem.h"
#include <stdexcept>

std::string AvailableState::getName() const { return "Available"; }
WorkItemState *AvailableState::clone() const { return new AvailableState(); }
bool AvailableState::isActive() const { return true; }
void AvailableState::assign(BasicWorkItem &item, Participant *p)
{
    if (!p)
        throw std::invalid_argument("AvailableState::assign needs a participant");
    item.setAssignee(p);
    item.setState(new AssignedState());
}
void AvailableState::escalate(BasicWorkItem &item)
{
    item.setState(new EscalatedState());
}
