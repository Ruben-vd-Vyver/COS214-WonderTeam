#ifndef ASSIGNEDSTATE_H
#define ASSIGNEDSTATE_H

#include "WorkItemState.h"

class AssignedState : public WorkItemState
{
public:
    std::string getName() const override;
    WorkItemState *clone() const override;
    bool isActive() const override;
    void assign(BasicWorkItem &item, Participant *p) override;
    void start(BasicWorkItem &item) override;
    void escalate(BasicWorkItem &item) override;
};

#endif // ASSIGNEDSTATE_H
