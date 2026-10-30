#include "EscalatedState.h"
#include "AssignedState.h"
#include "BasicWorkItem.h"
#include <stdexcept>

std::string EscalatedState::getName() const { return "Escalated"; }
WorkItemState *EscalatedState::clone() const { return new EscalatedState(); }
bool EscalatedState::isActive() const { return true; }
void EscalatedState::assign(BasicWorkItem &item, Participant *p)
{
    if (!p)
        throw std::invalid_argument("EscalatedState::assign needs a participant");
    item.setAssignee(p);
    item.setState(new AssignedState());
}
