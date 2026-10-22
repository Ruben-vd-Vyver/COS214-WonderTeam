#include "InProgressState.h"
#include "CompletedState.h"
#include "AwaitingApprovalState.h"
#include "RejectedState.h"
#include "EscalatedState.h"
#include "Task.h"

std::string InProgressState::getName() const { return "InProgress"; }
bool InProgressState::complete(Task &t)
{
    changeState(t, new CompletedState());
    return true;
}
bool InProgressState::requestApproval(Task &t)
{
    changeState(t, new AwaitingApprovalState());
    return true;
}
bool InProgressState::reject(Task &t, const std::string &)
{
    changeState(t, new RejectedState());
    return true;
}
bool InProgressState::escalate(Task &t)
{
    changeState(t, new EscalatedState());
    return true;
}