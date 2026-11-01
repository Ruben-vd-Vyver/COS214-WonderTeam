#include "AwaitingApprovalState.h"
#include "RejectedState.h"
#include "BasicWorkItem.h"
#include "ApprovalRequest.h"
#include "ApprovalHandler.h"
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
    const Task& task = item.getTask();

    if (task.requiresApproval(trigger_)) {
        ApprovalHandler* chain = task.getApprovalChain(trigger_);
        
        if (chain != nullptr) {
        
            Participant* requester = item.getAssignee();
            int requiredLevel = task.getRequiredLevel(trigger_);

            ApprovalRequest req(&item, requester, requiredLevel);
            chain->handle(req);

            if (req.isApproved()) {
               
                WorkItemState *next = onApproved_.release();
                item.setState(next);
            } else {
                item.setState(new RejectedState());
            }
            return;
        }
    }

    WorkItemState *next = onApproved_.release();
    item.setState(next);
}
void AwaitingApprovalState::reject(BasicWorkItem &item, const std::string &)
{
    item.setState(new RejectedState());
}
