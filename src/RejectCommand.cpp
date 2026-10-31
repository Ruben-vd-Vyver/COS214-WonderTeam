#include "../include/RejectCommand.h"
#include <sstream>
#include <utility>

RejectCommand::RejectCommand(WorkItem* workItem, std::string reason)
    : WorkItemCommand(workItem, nullptr), reason_(std::move(reason)) {}

bool RejectCommand::execute() {
    if (hasExecuted_)
        return false;

    saveSnapshot();
    workItem_->reject(reason_);
    hasExecuted_ = true;
    return true;
}

bool RejectCommand::undo() {
    if (!hasExecuted_)
        return false;

    if (!restoreSnapshot())
        return false;
    hasExecuted_ = false;
    return true;
}

std::string RejectCommand::describe() {
    std::stringstream description;
    description << "Reject Command\nInvolved WorkItem:\n"
                << workItem_->getId() << " - " << workItem_->getName()
                << "\nReason: " << reason_ << "\n";
    description << (hasExecuted_ ? "Status: HAS EXECUTED\n" : "Status: HAS NOT EXECUTED\n");
    return description.str();
}
