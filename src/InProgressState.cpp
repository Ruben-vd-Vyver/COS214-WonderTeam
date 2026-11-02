#include "InProgressState.h"
#include "CompletedState.h"
#include "RejectedState.h"
#include "EscalatedState.h"
#include "BasicWorkItem.h"

std::string InProgressState::getName() const { return "InProgress"; }
WorkItemState *InProgressState::clone() const { return new InProgressState(); }
bool InProgressState::isActive() const { return true; }
void InProgressState::complete(BasicWorkItem &item)
{
    item.advanceTo(new CompletedState(), "complete");
}
void InProgressState::reject(BasicWorkItem &item, const std::string &)
{
    item.setState(new RejectedState());
}
void InProgressState::escalate(BasicWorkItem &item)
{
    item.setState(new EscalatedState());
}
