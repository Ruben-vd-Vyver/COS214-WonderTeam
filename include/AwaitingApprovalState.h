#ifndef AWAITINGAPPROVALSTATE_H
#define AWAITINGAPPROVALSTATE_H

#include <memory>
#include "WorkItemState.h"

/**
 * @brief Represents the awaiting approval state abstraction in the TIVIDY workflow system.
 */
class AwaitingApprovalState : public WorkItemState
{
private:
    std::unique_ptr<WorkItemState> onApproved_;
    std::string trigger_;

public:
    /**
     * @brief Constructs an object of this type.
     * @param onApproved Value or object supplied by the caller.
     * @param trigger Value or object supplied by the caller.
     */
    AwaitingApprovalState(WorkItemState *onApproved, const std::string &trigger);
    /**
     * @brief Destroys this object and releases owned resources.
     */
    ~AwaitingApprovalState();
    /**
     * @brief Returns the name.
     * @return Result produced by the operation.
     */
    std::string getName() const override;
    /**
     * @brief Performs the clone operation.
     * @return Result produced by the operation.
     */
    WorkItemState *clone() const override;
    /**
     * @brief Reports whether the requested condition is satisfied.
     * @return Result produced by the operation.
     */
    bool isActive() const override;
    /**
     * @brief Performs the approve operation.
     * @param item Value or object supplied by the caller.
     */
    void approve(BasicWorkItem &item) override;
    /**
     * @brief Rejects the operation and records the rejection reason.
     * @param item Value or object supplied by the caller.
     * @param reason Value or object supplied by the caller.
     */
    void reject(BasicWorkItem &item, const std::string &reason) override;
};

#endif // AWAITINGAPPROVALSTATE_H
