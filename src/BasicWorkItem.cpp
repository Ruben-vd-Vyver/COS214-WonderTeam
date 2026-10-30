#include "BasicWorkItem.h"
#include "PendingState.h"
#include "AwaitingApprovalState.h"
#include "WorkItemMemento.h"
#include "WorkflowSubject.h"
#include "WorkflowEvent.h"
#include <stdexcept>

const Task &BasicWorkItem::requireSpec(const std::shared_ptr<const Task> &spec)
{
    if (!spec)
        throw std::invalid_argument("BasicWorkItem needs a task specification");
    return *spec;
}

BasicWorkItem::BasicWorkItem(std::string id, std::shared_ptr<const Task> spec)
    : WorkItem(std::move(id), requireSpec(spec).getName()),
      task_(spec),
      state_(new PendingState()),
      assignee_(nullptr),
      subject_(nullptr) {}

BasicWorkItem::~BasicWorkItem() {}

void BasicWorkItem::assign(Participant *p) { state_->assign(*this, p); }
void BasicWorkItem::start() { state_->start(*this); }
void BasicWorkItem::complete() { state_->complete(*this); }
void BasicWorkItem::reject(const std::string &reason) { state_->reject(*this, reason); }
void BasicWorkItem::escalate() { state_->escalate(*this); }
void BasicWorkItem::approve() { state_->approve(*this); }
void BasicWorkItem::activate() { state_->activate(*this); }

bool BasicWorkItem::isComplete() const { return state_->isComplete(); }
bool BasicWorkItem::isActive() const { return state_->isActive(); }
void BasicWorkItem::bind(WorkflowSubject *s) { subject_ = s; }

const Task &BasicWorkItem::getTask() const { return *task_; }
std::string BasicWorkItem::getRequiredRole() const { return task_->getRequiredRole(); }
std::string BasicWorkItem::getStateName() const { return state_->getName(); }
Participant *BasicWorkItem::getAssignee() const { return assignee_; }

void BasicWorkItem::setField(const std::string &key, const std::string &value) { payload_[key] = value; }

std::string BasicWorkItem::getField(const std::string &key) const
{
    std::map<std::string, std::string>::const_iterator it = payload_.find(key);
    return it == payload_.end() ? std::string() : it->second;
}

void BasicWorkItem::setState(WorkItemState *s)
{
    if (!s)
        throw std::invalid_argument("BasicWorkItem::setState needs a state");
    state_.reset(s);
    publish("state.changed");
}

void BasicWorkItem::setAssignee(Participant *p) { assignee_ = p; }
void BasicWorkItem::advanceTo(WorkItemState *next, const std::string &trigger)
{
    if (!next)
        throw std::invalid_argument("BasicWorkItem::advanceTo needs a state");
    if (task_->requiresApproval(trigger))
    {
        setState(new AwaitingApprovalState(next, trigger));
        publish("approval.requested");
    }
    else
        setState(next);
}

BasicWorkItem *BasicWorkItem::clone() const
{
    BasicWorkItem *copy = new BasicWorkItem(id_, task_);
    copy->state_.reset(state_->clone());
    copy->assignee_ = assignee_;
    copy->payload_ = payload_;
    return copy;
}

WorkItemMemento *BasicWorkItem::createMemento() const
{
    return new WorkItemMemento(state_->clone(), assignee_, payload_);
}

void BasicWorkItem::restore(const WorkItemMemento &m)
{
    if (!m.savedState_)
        throw std::invalid_argument("BasicWorkItem::restore: memento holds no state");
    state_.reset(m.savedState_->clone());
    assignee_ = m.assignee_;
    payload_ = m.payload_;
}

void BasicWorkItem::publish(const std::string &type)
{
    if (subject_)
        subject_->notify(WorkflowEvent(type, this, nullptr, getStateName()));
}
