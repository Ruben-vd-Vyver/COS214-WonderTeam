#include "TividyEngine.h"

#include <sstream>
#include <stdexcept>

#include "AssignCommand.h"
#include "AssignmentStrategy.h"
#include "CompleteCommand.h"
#include "DefinitionBuilder.h"
#include "Participant.h"
#include "RejectCommand.h"
#include "StartCommand.h"
#include "WorkAllocator.h"
#include "WorkItem.h"
#include "WorkflowDefinition.h"
#include "WorkflowDirector.h"
#include "WorkflowInstance.h"
#include "WorkflowNode.h"
#include "WorkflowObserver.h"

namespace
{
std::string msg(const std::string &op, const std::string &why)
{
    return "TividyEngine::" + op + ": " + why;
}

int countItems(WorkflowNode *n, bool onlyComplete)
{
    if (WorkItem *w = n->asWorkItem())
        return (!onlyComplete || w->isComplete()) ? 1 : 0;
    int total = 0;
    for (int i = 0; i < n->childCount(); ++i)
        total += countItems(n->child(i), onlyComplete);
    return total;
}

void describeTree(WorkflowNode *n, int depth, std::ostringstream &out)
{
    std::string pad(2 + depth * 2, ' ');
    if (WorkItem *w = n->asWorkItem())
    {
        Participant *a = w->getAssignee();
        out << pad << "- " << w->getId() << " '" << w->getName() << "' [" << w->getStateName() << "] -> "
            << (a ? a->getName() : "unassigned") << "\n";
        return;
    }
    out << pad << "+ " << n->getName() << (n->isComplete() ? " (complete)" : "") << "\n";
    for (int i = 0; i < n->childCount(); ++i)
        describeTree(n->child(i), depth + 1, out);
}
} // namespace

TividyEngine::TividyEngine() : definitionCount_(0), instanceCount_(0) {}

TividyEngine::~TividyEngine() {}

//private helpers
TividyEngine::Record &TividyEngine::record(const std::string &instanceId)
{
    auto it = records_.find(instanceId);
    if (it == records_.end())
        throw std::invalid_argument(msg("lookup", "unknown instance '" + instanceId + "'"));
    return it->second;
}

const TividyEngine::Record &TividyEngine::record(const std::string &instanceId) const
{
    auto it = records_.find(instanceId);
    if (it == records_.end())
        throw std::invalid_argument(msg("lookup", "unknown instance '" + instanceId + "'"));
    return it->second;
}

WorkItem &TividyEngine::itemRef(WorkflowInstance &inst, const std::string &itemId) const
{
    WorkItem *item = inst.findItem(itemId);
    if (!item)
        throw std::invalid_argument(msg("lookup", "instance '" + inst.getInstanceId() +
                                                      "' has no work item '" + itemId + "'"));
    return *item;
}

Participant &TividyEngine::actorRef(const std::string &name) const
{
    for (Participant *p : participants_)
        if (p->getName() == name)
            return *p;
    throw std::invalid_argument(msg("lookup", "unknown participant '" + name + "'"));
}

void TividyEngine::requireAssignee(const WorkItem &item, const Participant &who, const std::string &op) const
{
    if (item.getAssignee() != &who)
        throw std::runtime_error(msg(op, "'" + who.getName() + "' is not the assignee of " + item.getId() +
                                             " (" + item.getName() + ")"));
}

void TividyEngine::run(WorkflowInstance &inst, WorkItemCommand *cmd, const std::string &op) const
{
    try
    {
        if (!inst.perform(cmd))
            throw std::runtime_error(msg(op, "the command was refused"));
    }
    catch (const std::logic_error &e)
    {
        throw std::runtime_error(msg(op, e.what()));
    }
}

void TividyEngine::registerParticipant(Participant *p)
{
    if (!p)
        throw std::invalid_argument(msg("registerParticipant", "participant is null"));
    for (Participant *q : participants_)
    {
        if (q == p)
            return; // already registered
        if (q->getName() == p->getName())
            throw std::invalid_argument(msg("registerParticipant", "name '" + p->getName() + "' is already taken"));
    }
    participants_.push_back(p);
}

std::string TividyEngine::deploy(WorkflowDirector &d, const std::string &name, int version)
{
    std::string defName = name.empty() ? "workflow-" + std::to_string(definitionCount_ + 1) : name;
    std::string id = defName + "-v" + std::to_string(version);
    if (definitions_.count(id))
        throw std::invalid_argument(msg("deploy", "'" + id + "' is already deployed"));

    std::unique_ptr<WorkflowDefinition> def;
    try
    {
        DefinitionBuilder builder;
        builder.reset(defName, version);
        if (!d.construct(builder))
            throw std::runtime_error(msg("deploy", "the workflow script could not be parsed"));
        def.reset(builder.getResult());
    }
    catch (const char *why) // DefinitionBuilder reports misuse by throwing string literals
    {
        throw std::runtime_error(msg("deploy", why));
    }

    std::shared_ptr<WorkflowNode> root = def->getRoot();
    if (!root || countItems(root.get(), false) == 0)
        throw std::runtime_error(msg("deploy", "the workflow has no work items"));

    ++definitionCount_;
    definitions_[id] = std::move(def);
    return id;
}

std::string TividyEngine::startInstance(const std::string &definitionId,
                                        const std::vector<WorkflowObserver *> &observers)
{
    auto it = definitions_.find(definitionId);
    if (it == definitions_.end())
        throw std::invalid_argument(msg("startInstance", "unknown definition '" + definitionId + "'"));

    std::string id = "instance-" + std::to_string(instanceCount_ + 1);
    std::unique_ptr<WorkflowInstance> inst(it->second->createInstance(id));
    for (WorkflowObserver *o : observers)
        inst->attach(o); 
    inst->start();

    ++instanceCount_;
    Record &r = records_[id];
    r.instance = std::move(inst);
    r.definitionId = definitionId;
    return id;
}

void TividyEngine::setAssignment(const std::string &instanceId, AssignmentStrategy *s)
{
    if (!s)
        throw std::invalid_argument(msg("setAssignment", "strategy is null"));
    Record &r = record(instanceId);
    r.instance->getAllocator().setStrategy(s);
    r.hasStrategy = true;
}

void TividyEngine::subscribe(const std::string &instanceId, WorkflowObserver *o)
{
    if (!o)
        throw std::invalid_argument(msg("subscribe", "observer is null"));
    record(instanceId).instance->attach(o);
}

void TividyEngine::assignNext(const std::string &instanceId, const std::string &itemId)
{
    Record &r = record(instanceId);
    WorkflowInstance &inst = *r.instance;
    WorkItem &item = itemRef(inst, itemId);

    if (!r.hasStrategy)
        throw std::runtime_error(msg("assignNext", "no assignment strategy set for " + instanceId));
    if (participants_.empty())
        throw std::runtime_error(msg("assignNext", "no participants registered"));

    inst.getRoot()->activate();
    if (!item.isActive())
        throw std::runtime_error(msg("assignNext", item.getId() + " (" + item.getName() +
                                                       ") is not available yet, state: " + item.getStateName()));

    std::vector<Participant *> pool(participants_);

    Participant *chosen = inst.getAllocator().allocate(const_cast<Task &>(item.getTask()), pool);
    if (!chosen)
        throw std::runtime_error(msg("assignNext", "no eligible participant for '" + item.getName() +
                                                       "' (role " + item.getRequiredRole() + ")"));
    run(inst, new AssignCommand(&item, chosen), "assignNext");
}

void TividyEngine::startItem(const std::string &instanceId, const std::string &itemId, const std::string &actor)
{
    WorkflowInstance &inst = *record(instanceId).instance;
    WorkItem &item = itemRef(inst, itemId);
    requireAssignee(item, actorRef(actor), "startItem");
    run(inst, new StartCommand(&item), "startItem");
}

void TividyEngine::completeItem(const std::string &instanceId, const std::string &itemId, const std::string &actor)
{
    WorkflowInstance &inst = *record(instanceId).instance;
    WorkItem &item = itemRef(inst, itemId);
    requireAssignee(item, actorRef(actor), "completeItem");
    run(inst, new CompleteCommand(&item), "completeItem");
}

void TividyEngine::rejectItem(const std::string &instanceId, const std::string &itemId,
                              const std::string &actor, const std::string &reason)
{
    WorkflowInstance &inst = *record(instanceId).instance;
    WorkItem &item = itemRef(inst, itemId);
    requireAssignee(item, actorRef(actor), "rejectItem");
    run(inst, new RejectCommand(&item, reason), "rejectItem");
}

bool TividyEngine::undoLast(const std::string &instanceId)
{
    return record(instanceId).instance->undoLast();
}

std::string TividyEngine::statusReport(const std::string &instanceId) const
{
    const Record &r = record(instanceId);
    WorkflowNode *root = r.instance->getRoot();
    int total = countItems(root, false);
    int done = countItems(root, true);

    std::ostringstream out;
    out << "Instance " << instanceId << " of '" << r.definitionId << "' - "
        << (r.instance->isComplete() ? "COMPLETE" : "IN PROGRESS") << " (" << done << "/" << total
        << " items complete)\n";
    describeTree(root, 0, out);
    return out.str();
}
