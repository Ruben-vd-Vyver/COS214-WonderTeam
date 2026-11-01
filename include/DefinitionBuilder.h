#ifndef DEFINITIONBUILDER_H
#define DEFINITIONBUILDER_H

#include <memory>
#include <stack>
#include "WorkflowBuilder.h"

/**
 * @brief Represents the composite node abstraction in the TIVIDY workflow system.
 */
class CompositeNode;
/**
 * @brief Represents the task abstraction in the TIVIDY workflow system.
 */
class Task;

/**
 * @brief Represents the definition builder abstraction in the TIVIDY workflow system.
 */
class DefinitionBuilder : public WorkflowBuilder
{
private:
    std::unique_ptr<WorkflowDefinition> product_;
    /**
     * @brief Performs the root operation.
     * @param getResult Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    std::stack<CompositeNode*> openStages_; //bottom = root (owned here until getResult), others owned by their parent
    std::shared_ptr<Task> lastTask_;
    int stageCount_;
    int itemCount_;
    /**
     * @brief Performs the require product operation.
     */
    void requireProduct() const;
    /**
     * @brief Performs the current operation.
     * @return Result produced by the operation.
     */
    CompositeNode &current() const;
    /**
     * @brief Performs the discard root operation.
     */
    void discardRoot();

public:
    /**
     * @brief Constructs an object of this type.
     */
    DefinitionBuilder();
    /**
     * @brief Destroys this object and releases owned resources.
     */
    ~DefinitionBuilder();
    /**
     * @brief Performs the reset operation.
     * @param name Value or object supplied by the caller.
     * @param version Value or object supplied by the caller.
     */
    void reset(const std::string &name, int version) override;
    /**
     * @brief Performs the begin stage operation.
     * @param name Value or object supplied by the caller.
     * @param parallel Value or object supplied by the caller.
     */
    void beginStage(const std::string &name, bool parallel) override;
    /**
     * @brief Performs the end stage operation.
     */
    void endStage() override;
    /**
     * @brief Adds the task.
     * @param name Value or object supplied by the caller.
     * @param role Value or object supplied by the caller.
     */
    void addTask(const std::string &name, const std::string &role) override;
    /**
     * @brief Adds the custom item.
     * @param t Value or object supplied by the caller.
     */
    void addCustomItem(std::shared_ptr<WorkItem> t) override;
    /**
     * @brief Performs the require approval operation.
     * @param trigger Value or object supplied by the caller.
     * @param chain Value or object supplied by the caller.
     */
    void requireApproval(const std::string &trigger, ApprovalHandler *chain) override;
    /**
     * @brief Returns the result.
     * @return Result produced by the operation.
     */
    WorkflowDefinition *getResult() override;
};

#endif
