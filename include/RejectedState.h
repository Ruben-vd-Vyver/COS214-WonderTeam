#ifndef REJECTEDSTATE_H
#define REJECTEDSTATE_H

#include "WorkItemState.h"

/**
 * @brief Represents the rejected state abstraction in the TIVIDY workflow system.
 */
class RejectedState : public WorkItemState
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
};

#endif // REJECTEDSTATE_H
