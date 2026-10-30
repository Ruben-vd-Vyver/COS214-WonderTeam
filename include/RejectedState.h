#ifndef REJECTEDSTATE_H
#define REJECTEDSTATE_H

#include "WorkItemState.h"

class RejectedState : public WorkItemState
{
public:
    std::string getName() const override;
    WorkItemState *clone() const override;
};

#endif // REJECTEDSTATE_H
