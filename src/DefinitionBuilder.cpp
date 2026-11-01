#include "DefinitionBuilder.h"
#include "WorkflowDefinition.h"
#include "CompositeNode.h"
#include "Stage.h"
#include "Task.h"
#include "WorkItem.h"
#include "BasicWorkItem.h"

// Initializes counters used to generate stage and work item identifiers
DefinitionBuilder::DefinitionBuilder() : stageCount_(0), itemCount_(0) {}

// Releases resources owned by the builder before a result is transferred.
DefinitionBuilder::~DefinitionBuilder()
{
    discardRoot();
}

// The open-stage stack is non-owning. Its bottom element owns the complete
// untransferred tree through CompositeNode's raw-child ownership contract.
void DefinitionBuilder::discardRoot()
{
    if (openStages_.empty())
        return;

    while (openStages_.size() > 1)
        openStages_.pop();

    CompositeNode *root = openStages_.top();
    openStages_.pop();
    delete root;
}

// Ensures the builder has an active workflow product
void DefinitionBuilder::requireProduct() const
{
    if (!product_)
        throw std::string("DefinitionBuilder: call reset() first");
}

// Returns the currently open stage being constructed
CompositeNode &DefinitionBuilder::current() const
{
    return *openStages_.top();
}

// Starts a new workflow definition and resets the builder state
void DefinitionBuilder::reset(const std::string &name, int version)
{
    discardRoot();
    product_.reset(new WorkflowDefinition(name, version));
    lastTask_.reset();
    stageCount_ = 0;
    itemCount_ = 0;

    CompositeNode *root = new Stage("stage-" + std::to_string(stageCount_++), name, false);
    openStages_.push(root);
}

// Opens a new stage and adds it to the current stage hierarchy
void DefinitionBuilder::beginStage(const std::string &name, bool parallel)
{
    requireProduct();
    std::unique_ptr<CompositeNode> stage(new Stage("stage-" + std::to_string(stageCount_++), name, parallel));
    current().add(stage.get());
    openStages_.push(stage.get());
    stage.release(); // parent CompositeNode now owns the stage
    lastTask_.reset();
}

// Closes the current stage and returns construction to its parent
void DefinitionBuilder::endStage()
{
    requireProduct();
    if (openStages_.size() <= 1)
        throw std::string("DefinitionBuilder: endStage() without an open stage");
    openStages_.pop();
    lastTask_.reset();
}

// Creates a task and adds a corresponding work item to the current stage
void DefinitionBuilder::addTask(const std::string &name, const std::string &role)
{
    requireProduct();
    std::shared_ptr<Task> spec = std::make_shared<Task>(name, role);
    product_->addTask(spec);
    std::unique_ptr<BasicWorkItem> item(new BasicWorkItem("item-" + std::to_string(itemCount_++), spec));
    current().add(item.get());
    item.release(); // parent CompositeNode now owns the item
    lastTask_ = spec;
}

// Adds a caller provided work item to the current stage
void DefinitionBuilder::addCustomItem(std::shared_ptr<WorkItem> t)
{
    requireProduct();
    if (!t)
        throw std::string("DefinitionBuilder: addCustomItem needs an item");
    // CompositeNode owns raw child pointers, so add an independent clone of
    // the supplied shared item rather than creating conflicting ownership.
    std::unique_ptr<WorkItem> clone(t->clone());
    current().add(clone.get());
    clone.release(); // parent CompositeNode now owns the clone
    lastTask_.reset();
}

// Adds an approval handler to the task most recently created by the builder
void DefinitionBuilder::requireApproval(const std::string &trigger, ApprovalHandler *chain)
{
    requireProduct();
    if (!lastTask_)
        throw std::string("DefinitionBuilder: requireApproval() must directly follow addTask()");
    if (!chain)
        throw std::string("DefinitionBuilder: requireApproval needs a handler chain");
    lastTask_->requireApproval(trigger, chain);
    product_->addChain(chain);
}

// Validates and returns the completed workflow definition
WorkflowDefinition *DefinitionBuilder::getResult()
{
    requireProduct();
    if (openStages_.size() != 1)
        throw std::string("DefinitionBuilder: getResult() with a stage still open");
    product_->setRoot(std::shared_ptr<WorkflowNode>(openStages_.top()));
    while (!openStages_.empty())
        openStages_.pop();
    lastTask_.reset();
    return product_.release();
}
