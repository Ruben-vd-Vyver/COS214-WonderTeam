#include "WorkItemState.h"
#include <stdexcept>

void WorkItemState::activate(BasicWorkItem &) {}
void WorkItemState::assign(BasicWorkItem &, Participant *) { illegal("assign"); }
void WorkItemState::start(BasicWorkItem &) { illegal("start"); }
void WorkItemState::complete(BasicWorkItem &) { illegal("complete"); }
void WorkItemState::reject(BasicWorkItem &, const std::string &) { illegal("reject"); }
void WorkItemState::escalate(BasicWorkItem &) { illegal("escalate"); }
void WorkItemState::approve(BasicWorkItem &) { illegal("approve"); }
bool WorkItemState::isActive() const { return false; }
bool WorkItemState::isComplete() const { return false; }

void WorkItemState::illegal(const std::string &action) const
{
    throw std::logic_error("cannot " + action + " a work item that is " + getName());
}
