#ifndef PENDINGSTATE_H
#define PENDINGSTATE_H

#include "WorkItemState.h"

class PendingState : public WorkItemState
{
public:
    std::string getName() const override;
    WorkItemState *clone() const override;
    void activate(BasicWorkItem &item) override;
};

#endif // PENDINGSTATE_H
