#ifndef ESCALATEDSTATE_H
#define ESCALATEDSTATE_H

#include "WorkItemState.h"

/**
 * @brief Represents the escalated state abstraction in the TIVIDY workflow system.
 */
class EscalatedState : public WorkItemState
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
};

#endif // ESCALATEDSTATE_H
