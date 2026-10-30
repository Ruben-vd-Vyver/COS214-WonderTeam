#ifndef INPROGRESSSTATE_H
#define INPROGRESSSTATE_H

#include "WorkItemState.h"

class InProgressState : public WorkItemState
{
public:
    std::string getName() const override;
    WorkItemState *clone() const override;
    bool isActive() const override;
    void complete(BasicWorkItem &item) override;
    void reject(BasicWorkItem &item, const std::string &reason) override;
    void escalate(BasicWorkItem &item) override;
};

#endif // INPROGRESSSTATE_H
