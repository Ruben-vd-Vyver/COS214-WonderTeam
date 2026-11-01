#ifndef DEFINITIONBUILDER_H
#define DEFINITIONBUILDER_H

#include <memory>
#include <stack>
#include "WorkflowBuilder.h"

class CompositeNode;
class Task;

class DefinitionBuilder : public WorkflowBuilder
{
private:
    std::unique_ptr<WorkflowDefinition> product_;
    std::stack<CompositeNode*> openStages_; //bottom = root (owned here until getResult), others owned by their parent
    std::shared_ptr<Task> lastTask_;
    int stageCount_;
    int itemCount_;
    void requireProduct() const;
    CompositeNode &current() const;
    void discardRoot();

public:
    DefinitionBuilder();
    ~DefinitionBuilder();
    void reset(const std::string &name, int version) override;
    void beginStage(const std::string &name, bool parallel) override;
    void endStage() override;
    void addTask(const std::string &name, const std::string &role) override;
    void addCustomItem(std::shared_ptr<WorkItem> t) override;
    void requireApproval(const std::string &trigger, ApprovalHandler *chain) override;
    WorkflowDefinition *getResult() override;
};

#endif
