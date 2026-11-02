#ifndef BASICWORKITEM_H
#define BASICWORKITEM_H

#include <map>
#include <memory>
#include <string>
#include "WorkItem.h"
#include "WorkItemState.h"

/**
 * @brief Represents the participant abstraction in the TIVIDY workflow system.
 */
class Participant;
/**
 * @brief Represents the workflow subject abstraction in the TIVIDY workflow system.
 */
class WorkflowSubject;
/**
 * @brief Represents the basic work item abstraction in the TIVIDY workflow system.
 */
class BasicWorkItem : public WorkItem
{
private:
    std::shared_ptr<const Task> task_;
    std::map<std::string, std::string> payload_;
    std::unique_ptr<WorkItemState> state_;
    Participant *assignee_;
    WorkflowSubject *subject_;
    /**
     * @brief Performs the require spec operation.
     * @param spec Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    static const Task &requireSpec(const std::shared_ptr<const Task> &spec);

protected:
    /**
     * @brief Performs the publish operation.
     * @param type Value or object supplied by the caller.
     */
    virtual void publish(const std::string &type);

public:
    /**
     * @brief Constructs an object of this type.
     * @param id Value or object supplied by the caller.
     * @param spec Value or object supplied by the caller.
     */
    BasicWorkItem(std::string id, std::shared_ptr<const Task> spec);
    /**
     * @brief Destroys this object and releases owned resources.
     */
    ~BasicWorkItem();

    /**
     * @brief Performs the assign operation.
     * @param p Value or object supplied by the caller.
     */
    void assign(Participant *p) override;
    /**
     * @brief Starts the requested workflow operation.
     */
    void start() override;
    /**
     * @brief Completes the requested workflow operation.
     */
    void complete() override;
    /**
     * @brief Rejects the operation and records the rejection reason.
     * @param reason Value or object supplied by the caller.
     */
    void reject(const std::string &reason) override;
    /**
     * @brief Performs the escalate operation.
     */
    void escalate() override;
    /**
     * @brief Performs the approve operation.
     */
    void approve() override;

    /**
     * @brief Performs the activate operation.
     */
    void activate() override;
    /**
     * @brief Reports whether the requested condition is satisfied.
     * @return Result produced by the operation.
     */
    bool isComplete() const override;
    /**
     * @brief Performs the bind operation.
     * @param s Value or object supplied by the caller.
     */
    void bind(WorkflowSubject *s) override;
    /**
     * @brief Performs the clone operation.
     * @return Result produced by the operation.
     */
    BasicWorkItem *clone() const override;

    /**
     * @brief Reports whether the requested condition is satisfied.
     * @return Result produced by the operation.
     */
    bool isActive() const override;
    /**
     * @brief Returns the task.
     * @return Result produced by the operation.
     */
    const Task &getTask() const override;
    /**
     * @brief Returns the required role.
     * @return Result produced by the operation.
     */
    std::string getRequiredRole() const override;
    /**
     * @brief Returns the state name.
     * @return Result produced by the operation.
     */
    std::string getStateName() const override;
    /**
     * @brief Returns the assignee.
     * @return Result produced by the operation.
     */
    Participant *getAssignee() const override;
    /**
     * @brief Sets the field.
     * @param key Value or object supplied by the caller.
     * @param value Value or object supplied by the caller.
     */
    void setField(const std::string &key, const std::string &value) override;
    /**
     * @brief Returns the field.
     * @param key Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    std::string getField(const std::string &key) const override;

    /**
     * @brief Sets the state.
     * @param s Value or object supplied by the caller.
     */
    void setState(WorkItemState *s);
    /**
     * @brief Sets the assignee.
     * @param p Value or object supplied by the caller.
     */
    void setAssignee(Participant *p);
    /**
     * @brief Performs the advance to operation.
     * @param next Value or object supplied by the caller.
     * @param trigger Value or object supplied by the caller.
     */
    void advanceTo(WorkItemState *next, const std::string &trigger);

    /**
     * @brief Creates the memento.
     * @return Result produced by the operation.
     */
    std::unique_ptr<WorkItemMemento> createMemento() const override;
    /**
     * @brief Performs the restore operation.
     * @param m Value or object supplied by the caller.
     */
    void restore(const WorkItemMemento &m) override;
};

#endif // BASICWORKITEM_H
