#ifndef REJECTEDSTATE_H
#define REJECTEDSTATE_H

#include "TaskState.h"

class RejectedState : public TaskState
{
public:
    std::string getName() const override;
    bool hasFailed() const override;

    // [Prototype]
    RejectedState *clone() const override;
};

#endif // REJECTEDSTATE_H
