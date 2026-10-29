#include "DefinitionBuilder.h"
#include "WorkflowDefinition.h"
#include "CompositeNode.h"
#include "Stage.h"
#include "Task.h"
#include "WorkItem.h"
#include "BasicWorkItem.h"

DefinitionBuilder::DefinitionBuilder() : stageCount_(0), itemCount_(0) {}

DefinitionBuilder::~DefinitionBuilder() {}

void DefinitionBuilder::requireProduct() const
{
    if (!product_)
        throw "DefinitionBuilder: call reset() first";
}

CompositeNode &DefinitionBuilder::current() const
{
    return *openStages_.top();
}

void DefinitionBuilder::reset(const std::string &name, int version)
{
    product_.reset(new WorkflowDefinition(name, version));
    while (!openStages_.empty())
        openStages_.pop();
    lastTask_.reset();
    stageCount_ = 0;
    itemCount_ = 0;

    std::shared_ptr<CompositeNode> root(new Stage("stage-" + std::to_string(stageCount_++), name, false));
    openStages_.push(root);
}

void DefinitionBuilder::beginStage(const std::string &name, bool parallel)
{
    requireProduct();
    std::shared_ptr<CompositeNode> stage(new Stage("stage-" + std::to_string(stageCount_++), name, parallel));
    current().add(stage);
    openStages_.push(stage);
    lastTask_.reset();
}

void DefinitionBuilder::endStage()
{
    requireProduct();
    if (openStages_.size() <= 1)
        throw "DefinitionBuilder: endStage() without an open stage";
    openStages_.pop();
    lastTask_.reset();
}

void DefinitionBuilder::addTask(const std::string &name, const std::string &role)
{
    requireProduct();
    std::shared_ptr<Task> spec = std::make_shared<Task>(name, role);
    product_->addTask(spec);
    current().add(std::make_shared<BasicWorkItem>("item-" + std::to_string(itemCount_++), spec));
    lastTask_ = spec;
}

void DefinitionBuilder::addCustomItem(std::shared_ptr<WorkItem> t)
{
    requireProduct();
    if (!t)
        throw "DefinitionBuilder: addCustomItem needs an item";
    current().add(t);
    lastTask_.reset();
}

void DefinitionBuilder::requireApproval(const std::string &trigger, ApprovalHandler *chain)
{
    requireProduct();
    if (!lastTask_)
        throw "DefinitionBuilder: requireApproval() must directly follow addTask()";
    if (!chain)
        throw "DefinitionBuilder: requireApproval needs a handler chain";
    lastTask_->requireApproval(trigger, chain);
    product_->addChain(chain);
}

WorkflowDefinition *DefinitionBuilder::getResult()
{
    requireProduct();
    if (openStages_.size() != 1)
        throw "DefinitionBuilder: getResult() with a stage still open";
    product_->setRoot(openStages_.top());
    while (!openStages_.empty())
        openStages_.pop();
    lastTask_.reset();
    return product_.release();
}
