#ifndef AVAILABLESTATE_H
#define AVAILABLESTATE_H

#include "WorkItemState.h"

/**
 * @brief Represents the available state abstraction in the TIVIDY workflow system.
 */
class AvailableState : public WorkItemState
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
     * @brief Performs the assign operation.
     * @param item Value or object supplied by the caller.
     * @param p Value or object supplied by the caller.
     */
    void assign(BasicWorkItem &item, Participant *p) override;
    /**
     * @brief Performs the escalate operation.
     * @param item Value or object supplied by the caller.
     */
    void escalate(BasicWorkItem &item) override;
};

#endif // AVAILABLESTATE_H
