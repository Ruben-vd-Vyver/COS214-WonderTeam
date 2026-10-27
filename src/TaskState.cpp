#include "TaskState.h"
#include "Task.h"

void TaskState::activate(Task &) {}
bool TaskState::assign(Task &, Participant *) { return false; }
bool TaskState::start(Task &) { return false; }
bool TaskState::complete(Task &) { return false; }
bool TaskState::requestApproval(Task &) { return false; }
bool TaskState::reject(Task &, const std::string &) { return false; }
bool TaskState::escalate(Task &) { return false; }
bool TaskState::isComplete() const { return false; }
bool TaskState::hasFailed() const { return false; }
void TaskState::changeState(Task &t, TaskState *next) { t.state_.reset(next); } 
void TaskState::setAssignee(Task &t, Participant *p) { t.assignee_ = p; }
void TaskState::markActivated(Task &t) { t.activated_ = true; }