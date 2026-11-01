#ifndef COMPLETEDSTATE_H
#define COMPLETEDSTATE_H

#include "WorkItemState.h"

class CompletedState : public WorkItemState
{
public:
    std::string getName() const override;
    WorkItemState *clone() const override;
    bool isComplete() const override;
};

#endif // COMPLETEDSTATE_H
