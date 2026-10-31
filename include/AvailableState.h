#ifndef AVAILABLESTATE_H
#define AVAILABLESTATE_H

#include "WorkItemState.h"

class AvailableState : public WorkItemState
{
public:
    std::string getName() const override;
    WorkItemState *clone() const override;
    bool isActive() const override;
    void assign(BasicWorkItem &item, Participant *p) override;
    void escalate(BasicWorkItem &item) override;
};

#endif // AVAILABLESTATE_H
