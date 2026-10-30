#include "../include/ApproveCommand.h"
#include <sstream>

ApproveCommand::ApproveCommand(WorkItem* workItem)
    : WorkItemCommand(workItem, nullptr) {}

bool ApproveCommand::execute() {
    if (hasExecuted_)
        return false;

    previousState_ = workItem_->createMemento();
    workItem_->approve();
    hasExecuted_ = true;
    return true;
}

bool ApproveCommand::undo() {
    if (!hasExecuted_)
        return false;

    workItem_->restore(previousState_);
    hasExecuted_ = false;
    return true;
}

std::string ApproveCommand::describe() {
    std::stringstream description;
    description << "Approve Command\nInvolved WorkItem:\n"
                << workItem_->getId() << " - " << workItem_->getName() << "\n";
    description << (hasExecuted_ ? "Status: HAS EXECUTED\n" : "Status: HAS NOT EXECUTED\n");
    return description.str();
}
