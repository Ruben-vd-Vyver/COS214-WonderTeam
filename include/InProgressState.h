#ifndef INPROGRESSSTATE_H
#define INPROGRESSSTATE_H

#include "TaskState.h"

class InProgressState : public TaskState
{
public:
    std::string getName() const override;
    bool complete(Task &t) override;                          // -> Completed
    bool requestApproval(Task &t) override;                   // -> AwaitingApproval
    bool reject(Task &t, const std::string &reason) override; // -> Rejected
    bool escalate(Task &t) override;                          // -> Escalated
};

#endif // INPROGRESSSTATE_H