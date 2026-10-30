#include "../include/StartCommand.h"
#include <sstream>

StartCommand::StartCommand(WorkItem* workItem)
    : WorkItemCommand(workItem, nullptr) {}

bool StartCommand::execute() {
    if (hasExecuted_)
        return false;

    previousState_ = workItem_->createMemento();
    workItem_->start();
    hasExecuted_ = true;
    return true;
}

bool StartCommand::undo() {
    if (!hasExecuted_)
        return false;

    workItem_->restore(previousState_);
    hasExecuted_ = false;
    return true;
}

std::string StartCommand::describe() {
    std::stringstream description;
    description << "Start Command\nInvolved WorkItem:\n"
                << workItem_->getId() << " - " << workItem_->getName() << "\n";
    description << (hasExecuted_ ? "Status: HAS EXECUTED\n" : "Status: HAS NOT EXECUTED\n");
    return description.str();
}
