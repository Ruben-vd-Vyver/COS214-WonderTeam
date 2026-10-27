#ifndef AWAITINGAPPROVALSTATE_H
#define AWAITINGAPPROVALSTATE_H

#include "TaskState.h"

class AwaitingApprovalState : public TaskState
{
public:
    std::string getName() const override;
    bool complete(Task &t) override;
    bool reject(Task &t, const std::string &reason) override;
    bool escalate(Task &t) override;

    // [Prototype]
    AwaitingApprovalState *clone() const override;
};

#endif // AWAITINGAPPROVALSTATE_H
