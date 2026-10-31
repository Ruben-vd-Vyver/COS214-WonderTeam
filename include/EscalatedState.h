#ifndef ESCALATEDSTATE_H
#define ESCALATEDSTATE_H

#include "WorkItemState.h"

class EscalatedState : public WorkItemState
{
public:
    std::string getName() const override;
    WorkItemState *clone() const override;
    bool isActive() const override;
    void assign(BasicWorkItem &item, Participant *p) override;
};

#endif // ESCALATEDSTATE_H
