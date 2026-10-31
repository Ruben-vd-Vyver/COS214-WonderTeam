#include "AwaitingApprovalState.h"
#include "RejectedState.h"
#include "BasicWorkItem.h"
#include <stdexcept>

AwaitingApprovalState::AwaitingApprovalState(WorkItemState *onApproved, const std::string &trigger) : onApproved_(onApproved), trigger_(trigger)
{
    if (!onApproved_)
        throw std::invalid_argument("AwaitingApprovalState needs a state to move to once approved");
}
AwaitingApprovalState::~AwaitingApprovalState() {}
std::string AwaitingApprovalState::getName() const { return "AwaitingApproval"; }
WorkItemState *AwaitingApprovalState::clone() const
{
    return new AwaitingApprovalState(onApproved_->clone(), trigger_);
}
bool AwaitingApprovalState::isActive() const { return true; }
void AwaitingApprovalState::approve(BasicWorkItem &item)
{
    WorkItemState *next = onApproved_.release();
    item.setState(next);
}
void AwaitingApprovalState::reject(BasicWorkItem &item, const std::string &)
{
    item.setState(new RejectedState());
}
