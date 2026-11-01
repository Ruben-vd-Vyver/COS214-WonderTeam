#ifndef COMPLETEDSTATE_H
#define COMPLETEDSTATE_H

#include "WorkItemState.h"

/**
 * @brief Represents the completed state abstraction in the TIVIDY workflow system.
 */
class CompletedState : public WorkItemState
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
    bool isComplete() const override;
};

#endif // COMPLETEDSTATE_H
