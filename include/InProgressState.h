#ifndef INPROGRESSSTATE_H
#define INPROGRESSSTATE_H

#include "WorkItemState.h"

/**
 * @brief Represents the in progress state abstraction in the TIVIDY workflow system.
 */
class InProgressState : public WorkItemState
{
public:
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
     * @brief Completes the requested workflow operation.
     * @param item Value or object supplied by the caller.
     */
    void complete(BasicWorkItem &item) override;
    /**
     * @brief Rejects the operation and records the rejection reason.
     * @param item Value or object supplied by the caller.
     * @param reason Value or object supplied by the caller.
     */
    void reject(BasicWorkItem &item, const std::string &reason) override;
    /**
     * @brief Performs the escalate operation.
     * @param item Value or object supplied by the caller.
     */
    void escalate(BasicWorkItem &item) override;
};

#endif // INPROGRESSSTATE_H
