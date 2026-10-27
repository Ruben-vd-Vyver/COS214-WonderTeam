#include "AwaitingApprovalState.h"
#include "CompletedState.h"
#include "RejectedState.h"
#include "EscalatedState.h"
#include "Task.h"

std::string AwaitingApprovalState::getName() const { return "AwaitingApproval"; }
bool AwaitingApprovalState::complete(Task &t)
{
    changeState(t, new CompletedState());
    return true;
}
bool AwaitingApprovalState::reject(Task &t, const std::string &)
{
    changeState(t, new RejectedState());
    return true;
}
bool AwaitingApprovalState::escalate(Task &t)
{
    changeState(t, new EscalatedState());
    return true;
}

// [Prototype] states hold no data, so a plain copy is a full copy
AwaitingApprovalState *AwaitingApprovalState::clone() const { return new AwaitingApprovalState(*this); }
