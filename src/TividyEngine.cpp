#include "TividyEngine.h"

#include <cctype>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "AssignCommand.h"
#include "BasicWorkItem.h"
#include "PriorityItem.h"
#include "SecuredItem.h"
#include "ValidatedItem.h"
#include "WorkItemDecorator.h"
#include "CompositeNode.h"
#include "ApproveCommand.h"
#include "ApprovalHandler.h"
#include "ApprovalRequest.h"
#include "AssignmentStrategy.h"
#include "LeastWorkloadAssignment.h"
#include "RoleBasedAssignment.h"
#include "RuleBasedAssignment.h"
#include "CompleteCommand.h"
#include "DefinitionBuilder.h"
#include "Participant.h"
#include "RejectCommand.h"
#include "StartCommand.h"
#include "Stage.h"
#include "SubProcess.h"
#include "Task.h"
#include "WorkAllocator.h"
#include "WorkItem.h"
#include "WorkflowDefinition.h"
#include "WorkflowDirector.h"
#include "WorkflowInstance.h"
#include "WorkflowNode.h"
#include "WorkflowIterator.h"
#include "WorkflowObserver.h"

namespace
{
std::string msg(const std::string &op, const std::string &why)
{
    return "TividyEngine::" + op + ": " + why;
}

WorkflowNode *findNode(WorkflowNode *node, const std::string &id)
{
    if (!node)
        return nullptr;
    if (node->getId() == id)
        return node;
    for (int i = 0; i < node->childCount(); ++i)
    {
        if (WorkflowNode *found = findNode(node->child(i), id))
            return found;
    }
    return nullptr;
}

bool containsNodeId(WorkflowNode *node, const std::string &id)
{
    return findNode(node, id) != nullptr;
}

std::string nextNodeId(WorkflowNode *root, const std::string &prefix)
{
    for (unsigned long long index = 0;; ++index)
    {
        const std::string candidate = prefix + "-" + std::to_string(index);
        if (!containsNodeId(root, candidate))
            return candidate;
    }
}

CompositeNode &requireComposite(WorkflowNode *root, const std::string &id,
                                const std::string &operation)
{
    WorkflowNode *node = findNode(root, id);
    CompositeNode *composite = dynamic_cast<CompositeNode *>(node);
    if (!composite)
        throw std::invalid_argument(msg(operation, "parent '" + id + "' is not a composite node"));
    return *composite;
}

std::string lowerAscii(const std::string &value)
{
    std::string result(value);
    for (std::size_t i = 0; i < result.size(); ++i)
        result[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(result[i])));
    return result;
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

void collectDecoratorInfo(WorkItem *item, std::vector<std::string> &decorators)
{
    WorkItemDecorator *wrapper = dynamic_cast<WorkItemDecorator *>(item);
    if (!wrapper)
        return;

    if (dynamic_cast<ValidatedItem *>(wrapper))
        decorators.push_back("Validated");
    else if (SecuredItem *secured = dynamic_cast<SecuredItem *>(wrapper))
    {
        std::ostringstream label;
        label << "Secured(";
        const std::vector<std::string> &roles = secured->getPermittedRoles();
        for (std::size_t i = 0; i < roles.size(); ++i)
        {
            if (i) label << ", ";
            label << roles[i];
        }
        label << ")";
        decorators.push_back(label.str());
    }
    else if (PriorityItem *priority = dynamic_cast<PriorityItem *>(wrapper))
        decorators.push_back("Priority " + std::to_string(priority->getPriority()));

    collectDecoratorInfo(wrapper->wrappedItem(), decorators);
}

TividyEngine::WorkflowEntry makeWorkflowEntry(WorkflowNode *node, int depth)
{
    TividyEngine::WorkflowEntry entry;
    entry.depth = depth;
    entry.id = node->getId();
    entry.name = node->getName();

    WorkItem *item = node->asWorkItem();
    entry.isWorkItem = (item != nullptr);
    if (item)
    {
        Participant *assignee = item->getAssignee();
        entry.state = item->getStateName();
        entry.assignee = assignee ? assignee->getName() : "unassigned";
        entry.requiredRole = item->getRequiredRole();
        const std::vector<std::string> &required = item->getTask().getRequiredFields();
        for (std::vector<std::string>::const_iterator it = required.begin(); it != required.end(); ++it)
            entry.requiredFields.push_back(std::make_pair(*it, item->getField(*it)));
        collectDecoratorInfo(item, entry.decorators);
    }
    else
        entry.state = node->isComplete() ? "Complete" : "In progress";

    return entry;
}

std::unique_ptr<WorkItem> wrapPriority(std::unique_ptr<WorkItem> inner, int priority)
{
    WorkItem *raw = inner.release();
    try { return std::unique_ptr<WorkItem>(new PriorityItem(raw, priority)); }
    catch (...) { delete raw; throw; }
}

std::unique_ptr<WorkItem> wrapSecured(std::unique_ptr<WorkItem> inner,
                                      const std::vector<std::string> &roles)
{
    WorkItem *raw = inner.release();
    try { return std::unique_ptr<WorkItem>(new SecuredItem(raw, roles)); }
    catch (...) { delete raw; throw; }
}

std::unique_ptr<WorkItem> wrapValidated(std::unique_ptr<WorkItem> inner)
{
    WorkItem *raw = inner.release();
    try { return std::unique_ptr<WorkItem>(new ValidatedItem(raw)); }
    catch (...) { delete raw; throw; }
}

std::unique_ptr<WorkItem> makeDecoratedWorkItem(const std::string &id,
                                                const std::shared_ptr<Task> &specification,
                                                const WorkItemDecoratorOptions &options)
{
    std::unique_ptr<WorkItem> item(new BasicWorkItem(id, specification));
    if (options.usePriority)
        item = wrapPriority(std::move(item), options.priority);
    if (!options.permittedRoles.empty())
        item = wrapSecured(std::move(item), options.permittedRoles);
    if (options.validateRequiredFields)
        item = wrapValidated(std::move(item));
    return item;
}

void collectWorkflowEntries(WorkflowNode *node, int depth,
                            std::vector<TividyEngine::WorkflowEntry> &entries)
{
    if (!node)
        return;
    entries.push_back(makeWorkflowEntry(node, depth));
    for (int i = 0; i < node->childCount(); ++i)
        collectWorkflowEntries(node->child(i), depth + 1, entries);
}

bool findNodePath(WorkflowNode *current, WorkflowNode *target,
                  std::vector<WorkflowNode *> &path)
{
    if (!current)
        return false;
    path.push_back(current);
    if (current == target)
        return true;

    for (int i = 0; i < current->childCount(); ++i)
    {
        if (findNodePath(current->child(i), target, path))
            return true;
    }

    path.pop_back();
    return false;
}

void explainPendingItem(WorkflowNode *root, TividyEngine::WorkflowEntry &entry)
{
    if (!root || !entry.isWorkItem || entry.state != "Pending")
        return;

    WorkItem *target = root->find(entry.id);
    if (!target)
        return;

    std::vector<WorkflowNode *> path;
    if (!findNodePath(root, target, path) || path.size() < 2)
        return;

    // Stage::activate() walks children in order. In a sequential Stage it
    // activates the first unfinished child and returns, so any later branch
    // is blocked by that first unfinished sibling. SubProcess and parallel
    // Stages activate all unfinished children and therefore do not block.
    for (std::size_t depth = 0; depth + 1 < path.size(); ++depth)
    {
        Stage *stage = dynamic_cast<Stage *>(path[depth]);
        if (!stage || stage->isParallel())
            continue;

        WorkflowNode *pathChild = path[depth + 1];
        int targetChildIndex = -1;
        int firstUnfinishedIndex = -1;
        for (int i = 0; i < stage->childCount(); ++i)
        {
            WorkflowNode *child = stage->child(i);
            if (child == pathChild)
                targetChildIndex = i;
            if (firstUnfinishedIndex < 0 && !child->isComplete())
                firstUnfinishedIndex = i;
        }

        if (targetChildIndex >= 0 && firstUnfinishedIndex >= 0 &&
            firstUnfinishedIndex < targetChildIndex)
        {
            WorkflowNode *blocker = stage->child(firstUnfinishedIndex);
            entry.availabilityBlockerId = blocker->getId();
            entry.availabilityBlockerName = blocker->getName();
            entry.availabilityStageName = stage->getName();

            if (WorkItem *blockingItem = blocker->asWorkItem())
                entry.availabilityBlockerState = blockingItem->getStateName();
            else
            {
                const int total = countItems(blocker, false);
                const int completed = countItems(blocker, true);
                if (total == 0)
                    entry.availabilityBlockerState = "empty composite; add subtasks to continue";
                else
                {
                    std::ostringstream state;
                    state << "incomplete composite, " << completed << "/" << total << " tasks complete";
                    entry.availabilityBlockerState = state.str();
                }
            }
            return; // Report the first gate the workflow must clear.
        }
    }
}

void annotateAvailabilityReasons(WorkflowNode *root,
                                 std::vector<TividyEngine::WorkflowEntry> &entries)
{
    for (std::size_t i = 0; i < entries.size(); ++i)
        explainPendingItem(root, entries[i]);
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
    r.ownedStrategy.reset(); // caller retains ownership for custom strategies
    r.strategyName = "Custom";
    r.hasStrategy = true;
}

void TividyEngine::setAssignmentMode(const std::string &instanceId, const std::string &mode)
{
    const std::string normalized = lowerAscii(mode);
    std::unique_ptr<AssignmentStrategy> replacement;
    if (normalized == "role" || normalized == "role-based")
        replacement.reset(new RoleBasedAssignment());
    else if (normalized == "workload" || normalized == "least-workload")
        replacement.reset(new LeastWorkloadAssignment());
    else if (normalized == "rule" || normalized == "rule-based")
        replacement.reset(new RuleBasedAssignment());
    else
        throw std::invalid_argument(msg("setAssignmentMode", "mode must be role, workload, or rule"));

    Record &r = record(instanceId);
    r.instance->getAllocator().setStrategy(replacement.get());
    r.ownedStrategy = std::move(replacement);
    r.strategyName = normalized == "role-based" ? "role" :
                     normalized == "least-workload" ? "workload" :
                     normalized == "rule-based" ? "rule" : normalized;
    r.hasStrategy = true;
}

std::string TividyEngine::assignmentMode(const std::string &instanceId) const
{
    const Record &r = record(instanceId);
    return r.strategyName.empty() ? "Not configured" : r.strategyName;
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

void TividyEngine::assignItem(const std::string &instanceId, const std::string &itemId,
                              const std::string &participantName)
{
    WorkflowInstance &inst = *record(instanceId).instance;
    WorkItem &item = itemRef(inst, itemId);
    Participant &participant = actorRef(participantName);

    inst.getRoot()->activate();
    if (!item.isActive())
        throw std::runtime_error(msg("assignItem", item.getId() + " (" + item.getName() +
                                                    ") is not available yet, state: " + item.getStateName()));
    if (!participant.hasRole(item.getRequiredRole()))
        throw std::runtime_error(msg("assignItem", "participant '" + participant.getName() +
                                                    "' does not have required role '" + item.getRequiredRole() + "'"));

    run(inst, new AssignCommand(&item, &participant), "assignItem");
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

    // Re-run activation after completion so the next child of a sequential
    // Stage becomes Available immediately, without requiring an attempted
    // assignment to trigger activation as a side effect.
    inst.getRoot()->activate();
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

std::string TividyEngine::submitApproval(
    const std::string &instanceId,
    const std::string &itemId,
    const std::string &requesterName,
    const std::string &trigger
)
{
    WorkflowInstance &inst = *record(instanceId).instance;
    WorkItem &item = itemRef(inst, itemId);
    Participant &requester = actorRef(requesterName);

    if (item.getStateName() != "AwaitingApproval")
        throw std::runtime_error(
            msg(
                "submitApproval",
                "item '" + itemId +
                    "' is not awaiting approval (state: " +
                    item.getStateName() + ")"
            )
        );

    ApprovalHandler *chain = item.getTask().getApprovalChain(trigger);
    if (!chain)
        throw std::runtime_error(
            msg(
                "submitApproval",
                "no approval chain is configured for trigger '" +
                    trigger + "'"
            )
        );

    const int requiredLevel = item.getTask().getRequiredLevel(trigger);
    ApprovalRequest request(&item, &requester, requiredLevel);
    inst.requestApproval(&item);
    chain->handle(request);
    if (!request.isDecided())
        throw std::runtime_error(
            msg("submitApproval", "the approval chain did not make a decision")
        );

    if (request.isApproved())
    {
        run(inst, new ApproveCommand(&item), "submitApproval");
        inst.approve(&item);
    }
    else
    {
        run(inst, new RejectCommand(&item, request.getReason()), "submitApproval");
        inst.reject(&item);
    }

    // Refresh activation after the approval decision.
    inst.getRoot()->activate();
    return request.getReason();
}

std::vector<TividyEngine::WorkflowEntry> TividyEngine::workflowTree(const std::string &instanceId) const
{
    const Record &r = record(instanceId);
    std::vector<WorkflowEntry> entries;
    WorkflowNode *root = r.instance->getRoot();
    collectWorkflowEntries(root, 0, entries);
    annotateAvailabilityReasons(root, entries);
    return entries;
}

std::vector<TividyEngine::WorkflowEntry> TividyEngine::workflowOverview(const std::string &instanceId) const
{
    const Record &r = record(instanceId);
    WorkflowNode *root = r.instance->getRoot();
    std::vector<WorkflowEntry> entries;
    if (!root)
        return entries;

    // The root itself is not yielded by CompositeNode::createIterator(); the
    // depth-first iterator yields every descendant, with its relative depth.
    entries.push_back(makeWorkflowEntry(root, 0));
    std::unique_ptr<WorkflowIterator> iterator(root->createIterator());
    for (iterator->first(); !iterator->isDone(); iterator->next())
    {
        WorkflowNode *node = iterator->current();
        if (node)
            entries.push_back(makeWorkflowEntry(node, iterator->currentDepth()));
    }
    annotateAvailabilityReasons(root, entries);
    return entries;
}

std::string TividyEngine::createTask(const std::string &instanceId, const std::string &name,
                                    const std::string &requiredRole,
                                    const std::vector<std::string> &requiredFields,
                                    const WorkItemDecoratorOptions &decorators)
{
    WorkflowInstance &inst = *record(instanceId).instance;
    WorkflowNode *root = inst.getRoot();
    if (name.empty())
        throw std::invalid_argument(msg("createTask", "task name cannot be empty"));
    if (requiredRole.empty())
        throw std::invalid_argument(msg("createTask", "required role cannot be empty"));

    CompositeNode *parent = dynamic_cast<CompositeNode *>(root);
    if (!parent)
        throw std::runtime_error(msg("createTask", "workflow root is not a composite node"));

    std::shared_ptr<Task> specification(new Task(name, requiredRole));
    for (std::size_t i = 0; i < requiredFields.size(); ++i)
    {
        if (!requiredFields[i].empty())
            specification->addRequiredField(requiredFields[i]);
    }

    const std::string id = nextNodeId(root, "item");
    std::unique_ptr<WorkItem> item = makeDecoratedWorkItem(id, specification, decorators);
    WorkItem *rawItem = item.get();
    parent->add(rawItem);
    item.release(); // CompositeNode now owns the outer decorator (and its wrapped item).

    // Start activation at the workflow root so enclosing sequential Stages
    // continue to enforce their ordering policy after a runtime insertion.
    root->activate();
    inst.createWorkItem(rawItem);
    return id;
}

std::string TividyEngine::createSubtask(const std::string &instanceId,
                                        const std::string &parentCompositeId,
                                        const std::string &name,
                                        const std::string &requiredRole,
                                        const std::vector<std::string> &requiredFields,
                                        const WorkItemDecoratorOptions &decorators)
{
    WorkflowInstance &inst = *record(instanceId).instance;
    WorkflowNode *root = inst.getRoot();
    if (name.empty())
        throw std::invalid_argument(msg("createSubtask", "subtask name cannot be empty"));
    if (requiredRole.empty())
        throw std::invalid_argument(msg("createSubtask", "required role cannot be empty"));

    CompositeNode &parent = requireComposite(root, parentCompositeId, "createSubtask");
    std::shared_ptr<Task> specification(new Task(name, requiredRole));
    for (std::size_t i = 0; i < requiredFields.size(); ++i)
    {
        if (!requiredFields[i].empty())
            specification->addRequiredField(requiredFields[i]);
    }

    const std::string id = nextNodeId(root, "item");
    std::unique_ptr<WorkItem> item = makeDecoratedWorkItem(id, specification, decorators);
    WorkItem *rawItem = item.get();
    parent.add(rawItem);
    item.release(); // The selected composite owns the outer decorator and wrapped item.

    // Do not activate a nested container in isolation: an ancestor sequential
    // Stage may not have reached this branch yet.
    root->activate();
    inst.createWorkItem(rawItem);
    return id;
}

std::string TividyEngine::createComposite(const std::string &instanceId,
                                          const std::string &parentCompositeId,
                                          const std::string &name,
                                          const std::string &type,
                                          bool parallel, int sourceVersion)
{
    WorkflowInstance &inst = *record(instanceId).instance;
    WorkflowNode *root = inst.getRoot();
    if (name.empty())
        throw std::invalid_argument(msg("createComposite", "composite name cannot be empty"));

    const std::string normalizedType = lowerAscii(type);
    if (normalizedType != "stage" && normalizedType != "subprocess")
        throw std::invalid_argument(msg("createComposite", "type must be 'stage' or 'subprocess'"));
    if (sourceVersion < 1)
        throw std::invalid_argument(msg("createComposite", "source version must be 1 or greater"));

    CompositeNode &parent = requireComposite(root, parentCompositeId, "createComposite");
    const std::string prefix = normalizedType == "stage" ? "stage" : "subprocess";
    const std::string id = nextNodeId(root, prefix);

    std::unique_ptr<CompositeNode> composite;
    if (normalizedType == "stage")
        composite.reset(new Stage(id, name, parallel));
    else
        composite.reset(new SubProcess(id, name, sourceVersion));

    parent.add(composite.get());
    composite.release(); // The parent CompositeNode now owns the new container.
    inst.structureChanged("CompositeCreated", id + " - " + name);
    root->activate();
    return id;
}

void TividyEngine::setItemField(const std::string &instanceId, const std::string &itemId,
                                const std::string &actor, const std::string &field,
                                const std::string &value)
{
    if (field.empty())
        throw std::invalid_argument(msg("setItemField", "field name cannot be empty"));
    WorkflowInstance &inst = *record(instanceId).instance;
    WorkItem &item = itemRef(inst, itemId);
    requireAssignee(item, actorRef(actor), "setItemField");
    if (item.getStateName() != "InProgress")
        throw std::runtime_error(msg("setItemField", "fields can only be edited while the item is InProgress"));
    item.setField(field, value);
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
