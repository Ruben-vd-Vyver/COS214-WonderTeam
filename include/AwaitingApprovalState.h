#ifndef AWAITINGAPPROVALSTATE_H
#define AWAITINGAPPROVALSTATE_H

#include <memory>
#include "WorkItemState.h"

class AwaitingApprovalState : public WorkItemState
{
private:
    std::unique_ptr<WorkItemState> onApproved_;
    std::string trigger_;

public:
    AwaitingApprovalState(WorkItemState *onApproved, const std::string &trigger);
    ~AwaitingApprovalState();
    std::string getName() const override;
    WorkItemState *clone() const override;
    bool isActive() const override;
    void approve(BasicWorkItem &item) override;
    void reject(BasicWorkItem &item, const std::string &reason) override;
};

#endif // AWAITINGAPPROVALSTATE_H
