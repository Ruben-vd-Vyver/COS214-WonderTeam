#ifndef AWAITINGAPPROVALSTATE_H
#define AWAITINGAPPROVALSTATE_H

#include "TaskState.h"

class AwaitingApprovalState : public TaskState
{
public:
    std::string getName() const override;
    bool complete(Task &t) override; // -> InProgress
    bool reject(Task &t) override;   // -> Rejected
    bool escalate(Task &t) override; // -> Escalated
};

#endif // AWAITINGAPPROVALSTATE_H