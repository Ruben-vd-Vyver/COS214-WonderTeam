#ifndef WORKFLOWINSTANCE_H
#define WORKFLOWINSTANCE_H

#include <ctime>
#include <map>
#include <memory>
#include <string>

#include "WorkflowSubject.h"
#include "CommandHistory.h"
#include "WorkAllocator.h"

/**
 * @brief Represents the work item abstraction in the TIVIDY workflow system.
 */
class WorkItem;
/**
 * @brief Represents the work item command abstraction in the TIVIDY workflow system.
 */
class WorkItemCommand;
/**
 * @brief Represents the workflow node abstraction in the TIVIDY workflow system.
 */
class WorkflowNode;
/**
 * @brief Represents the workflow definition abstraction in the TIVIDY workflow system.
 */
class WorkflowDefinition;

// ConcreteSubject
/**
 * @brief Represents the workflow instance abstraction in the TIVIDY workflow system.
 */
class WorkflowInstance : public WorkflowSubject
{
private:
    std::string instanceId;
    std::map<std::string, std::string> data;
    std::unique_ptr<WorkflowNode> root_;
    CommandHistory history_;
    WorkAllocator allocator_;

    /**
     * @brief Performs the publish event operation.
     * @param type Value or object supplied by the caller.
     * @param item Value or object supplied by the caller.
     * @param detail Value or object supplied by the caller.
     * @param deadline Value or object supplied by the caller.
     * @param hasDeadline Value or object supplied by the caller.
     */
    void publishEvent(const std::string &type, WorkItem *item, const std::string &detail,std::time_t deadline = 0, bool hasDeadline = false);

public:
    /**
     * @brief Constructs an object of this type.
     * @param def Value or object supplied by the caller.
     * @param root Value or object supplied by the caller.
     * @param id Value or object supplied by the caller.
     */
    WorkflowInstance(const WorkflowDefinition &def, WorkflowNode *root, const std::string &id);
    /**
     * @brief Constructs an object of this type.
     * @param instanceId Value or object supplied by the caller.
     */
    explicit WorkflowInstance(const std::string &instanceId); // event-only instance, no tree
    /**
     * @brief Destroys this object and releases owned resources.
     */
    ~WorkflowInstance();
    /**
     * @brief Constructs an object of this type.
     */
    WorkflowInstance(const WorkflowInstance &) = delete;
    WorkflowInstance &operator=(const WorkflowInstance &) = delete;
    /**
     * @brief Returns the instance id.
     * @return Result produced by the operation.
     */
    const std::string &getInstanceId() const;
    /**
     * @brief Starts the requested workflow operation.
     */
    void start();

    /**
     * @brief Performs the perform operation.
     * @param cmd Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    bool perform(WorkItemCommand *cmd);
    /**
     * @brief Performs the undo last operation.
     * @return Result produced by the operation.
     */
    bool undoLast();
    /**
     * @brief Returns the root.
     * @return Result produced by the operation.
     */
    WorkflowNode *getRoot() const;
    /**
     * @brief Finds the item.
     * @param id Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    WorkItem *findItem(const std::string &id) const;
    /**
     * @brief Reports whether the requested condition is satisfied.
     * @return Result produced by the operation.
     */
    bool isComplete() const;
    /**
     * @brief Returns the allocator.
     * @return Result produced by the operation.
     */
    WorkAllocator &getAllocator();
    /**
     * @brief Sets the data.
     * @param key Value or object supplied by the caller.
     * @param value Value or object supplied by the caller.
     */
    void setData(const std::string &key, const std::string &value);
    /**
     * @brief Returns the data.
     * @param key Value or object supplied by the caller.
     * @param value Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    bool getData(const std::string &key, std::string &value) const;
    /**
     * @brief Returns the data.
     * @param key Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    std::string getData(const std::string &key) const; // empty string when the key is missing
    /**
     * @brief Creates the work item.
     * @param item Value or object supplied by the caller.
     */
    void createWorkItem(WorkItem *item);
    /**
     * @brief Publishes an event describing a structural workflow change.
     * @param eventType Type of structural change to publish.
     * @param detail Human-readable details about the change.
     */
    void structureChanged(const std::string &eventType, const std::string &detail);
    /**
     * @brief Performs the request approval operation.
     * @param item Value or object supplied by the caller.
     */
    void requestApproval(WorkItem *item);
    /**
     * @brief Performs the approve operation.
     * @param item Value or object supplied by the caller.
     */
    void approve(WorkItem *item);
    /**
     * @brief Rejects the operation and records the rejection reason.
     * @param item Value or object supplied by the caller.
     */
    void reject(WorkItem *item);
    /**
     * @brief Completes the requested workflow operation.
     * @param item Value or object supplied by the caller.
     */
    void completeWorkItem(WorkItem *item);
    /**
     * @brief Sets the deadline.
     * @param item Value or object supplied by the caller.
     * @param deadline Value or object supplied by the caller.
     */
    void setDeadline(WorkItem *item, std::time_t deadline);
};

#endif // WORKFLOWINSTANCE_H
