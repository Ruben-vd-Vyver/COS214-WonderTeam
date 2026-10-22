#ifndef PENDINGSTATE_H
#define PENDINGSTATE_H

#include "TaskState.h"

class PendingState : public TaskState
{
public:
    std::string getName() const override;
    void activate(Task &t) override; //-> Available
};

#endif // PENDINGSTATE_H