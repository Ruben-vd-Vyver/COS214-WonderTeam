#ifndef WORKITEMSTATE_H
#define WORKITEMSTATE_H

#include <string>

/**
 * @brief Represents the basic work item abstraction in the TIVIDY workflow system.
 */
class BasicWorkItem;
/**
 * @brief Represents the participant abstraction in the TIVIDY workflow system.
 */
class Participant;
/**
 * @brief Represents the work item state abstraction in the TIVIDY workflow system.
 */
class WorkItemState
{
public:
    /**
     * @brief Destroys this object and releases owned resources.
     */
    virtual ~WorkItemState() {}

    /**
     * @brief Returns the name.
     * @return Result produced by the operation.
     */
    virtual std::string getName() const = 0;
    /**
     * @brief Performs the clone operation.
     * @return Result produced by the operation.
     */
    virtual WorkItemState *clone() const = 0;

    /**
     * @brief Performs the activate operation.
     * @param item Value or object supplied by the caller.
     */
    virtual void activate(BasicWorkItem &item);
    /**
     * @brief Performs the assign operation.
     * @param item Value or object supplied by the caller.
     * @param p Value or object supplied by the caller.
     */
    virtual void assign(BasicWorkItem &item, Participant *p);
    /**
     * @brief Starts the requested workflow operation.
     * @param item Value or object supplied by the caller.
     */
    virtual void start(BasicWorkItem &item);
    /**
     * @brief Completes the requested workflow operation.
     * @param item Value or object supplied by the caller.
     */
    virtual void complete(BasicWorkItem &item);
    /**
     * @brief Rejects the operation and records the rejection reason.
     * @param item Value or object supplied by the caller.
     * @param reason Value or object supplied by the caller.
     */
    virtual void reject(BasicWorkItem &item, const std::string &reason);
    /**
     * @brief Performs the escalate operation.
     * @param item Value or object supplied by the caller.
     */
    virtual void escalate(BasicWorkItem &item);
    /**
     * @brief Performs the approve operation.
     * @param item Value or object supplied by the caller.
     */
    virtual void approve(BasicWorkItem &item);

    /**
     * @brief Reports whether the requested condition is satisfied.
     * @return Result produced by the operation.
     */
    virtual bool isActive() const;
    /**
     * @brief Reports whether the requested condition is satisfied.
     * @return Result produced by the operation.
     */
    virtual bool isComplete() const;

protected:
    /**
     * @brief Performs the illegal operation.
     * @param action Value or object supplied by the caller.
     */
    void illegal(const std::string &action) const;
};

#endif // WORKITEMSTATE_H
