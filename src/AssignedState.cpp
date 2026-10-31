#include "AssignedState.h"
#include "InProgressState.h"
#include "EscalatedState.h"
#include "BasicWorkItem.h"
#include <stdexcept>

std::string AssignedState::getName() const { return "Assigned"; }
WorkItemState *AssignedState::clone() const { return new AssignedState(); }
bool AssignedState::isActive() const { return true; }
void AssignedState::assign(BasicWorkItem &item, Participant *p)
{
    if (!p)
        throw std::invalid_argument("AssignedState::assign needs a participant");
    item.setAssignee(p);
}
void AssignedState::start(BasicWorkItem &item)
{
    item.setState(new InProgressState());
}
void AssignedState::escalate(BasicWorkItem &item)
{
    item.setState(new EscalatedState());
}
