#ifndef PENDINGSTATE_H
#define PENDINGSTATE_H

#include "WorkItemState.h"

/**
 * @brief Represents the pending state abstraction in the TIVIDY workflow system.
 */
class PendingState : public WorkItemState
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
     * @brief Performs the activate operation.
     * @param item Value or object supplied by the caller.
     */
    void activate(BasicWorkItem &item) override;
};

#endif // PENDINGSTATE_H
