#ifndef WORKFLOWBUILDER_H
#define WORKFLOWBUILDER_H

#include <memory>
#include <string>

/**
 * @brief Represents the work item abstraction in the TIVIDY workflow system.
 */
class WorkItem;
/**
 * @brief Represents the approval handler abstraction in the TIVIDY workflow system.
 */
class ApprovalHandler;
/**
 * @brief Represents the workflow definition abstraction in the TIVIDY workflow system.
 */
class WorkflowDefinition;

/**
 * @brief Represents the workflow builder abstraction in the TIVIDY workflow system.
 */
class WorkflowBuilder
{
public:
    /**
     * @brief Destroys this object and releases owned resources.
     */
    virtual ~WorkflowBuilder() {}
    /**
     * @brief Performs the reset operation.
     * @param name Value or object supplied by the caller.
     * @param version Value or object supplied by the caller.
     */
    virtual void reset(const std::string &name, int version) = 0;
    /**
     * @brief Performs the begin stage operation.
     * @param name Value or object supplied by the caller.
     * @param parallel Value or object supplied by the caller.
     */
    virtual void beginStage(const std::string &name, bool parallel) = 0;
    /**
     * @brief Performs the end stage operation.
     */
    virtual void endStage() = 0;
    /**
     * @brief Adds the task.
     * @param name Value or object supplied by the caller.
     * @param role Value or object supplied by the caller.
     */
    virtual void addTask(const std::string &name, const std::string &role) = 0;
    /**
     * @brief Adds the custom item.
     * @param t Value or object supplied by the caller.
     */
    virtual void addCustomItem(std::shared_ptr<WorkItem> t) = 0;
    /**
     * @brief Configures approval for the task most recently added.
     * @param trigger Event that requires approval.
     * @param chain Approval handler chain to evaluate.
     * @param requiredLevel Minimum authority level required.
     */
    virtual void requireApproval(
        const std::string &trigger,
        ApprovalHandler *chain,
        int requiredLevel
    ) = 0;
    /**
     * @brief Returns the result.
     * @return Result produced by the operation.
     */
    virtual WorkflowDefinition *getResult() = 0;
};

#endif
