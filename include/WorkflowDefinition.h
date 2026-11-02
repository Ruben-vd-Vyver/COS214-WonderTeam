#ifndef WORKFLOWDEFINITION_H
#define WORKFLOWDEFINITION_H

#include <memory>
#include <string>
#include <vector>

/**
 * @brief Represents the workflow node abstraction in the TIVIDY workflow system.
 */
class WorkflowNode;
/**
 * @brief Represents the workflow instance abstraction in the TIVIDY workflow system.
 */
class WorkflowInstance;
/**
 * @brief Represents the approval handler abstraction in the TIVIDY workflow system.
 */
class ApprovalHandler;
/**
 * @brief Represents the task abstraction in the TIVIDY workflow system.
 */
class Task;

/**
 * @brief Represents the workflow definition abstraction in the TIVIDY workflow system.
 */
class WorkflowDefinition
{
private:
    std::string name_;
    int version_;
    std::shared_ptr<WorkflowNode> root_;
    std::vector<std::shared_ptr<Task>> tasks_;
    std::vector<ApprovalHandler *> chains_;

public:
    /**
     * @brief Constructs an object of this type.
     * @param name Value or object supplied by the caller.
     * @param version Value or object supplied by the caller.
     */
    WorkflowDefinition(std::string name, int version);
    /**
     * @brief Sets the root.
     * @param root Value or object supplied by the caller.
     */
    void setRoot(std::shared_ptr<WorkflowNode> root);
    /**
     * @brief Returns the root.
     * @return Result produced by the operation.
     */
    std::shared_ptr<WorkflowNode> getRoot() const;
    /**
     * @brief Returns the name.
     * @return Result produced by the operation.
     */
    const std::string &getName() const;
    /**
     * @brief Returns the version.
     * @return Result produced by the operation.
     */
    int getVersion() const;
    /**
     * @brief Adds the task.
     * @param t Value or object supplied by the caller.
     */
    void addTask(std::shared_ptr<Task> t);
    /**
     * @brief Returns the tasks.
     * @return Result produced by the operation.
     */
    const std::vector<std::shared_ptr<Task>> &getTasks() const;
    /**
     * @brief Adds the chain.
     * @param h Value or object supplied by the caller.
     */
    void addChain(ApprovalHandler *h);
    /**
     * @brief Returns the chains.
     * @return Result produced by the operation.
     */
    const std::vector<ApprovalHandler *> &getChains() const;
    /**
     * @brief Creates the instance.
     * @param id Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    WorkflowInstance *createInstance(const std::string &id = "") const;
};

#endif
