#include "../include/CompleteCommand.h"
#include <sstream>

CompleteCommand::CompleteCommand(WorkItem* workItem)
    : WorkItemCommand(workItem, nullptr) {}

bool CompleteCommand::execute() {
    if (hasExecuted_)
        return false;

    saveSnapshot();
    workItem_->complete();
    hasExecuted_ = true;
    return true;
}

bool CompleteCommand::undo() {
    if (!hasExecuted_)
        return false;

    if (!restoreSnapshot())
        return false;
    hasExecuted_ = false;
    return true;
}

std::string CompleteCommand::describe() {
    std::stringstream description;
    description << "Complete Command\nInvolved WorkItem:\n"
                << workItem_->getId() << " - " << workItem_->getName() << "\n";
    description << (hasExecuted_ ? "Status: HAS EXECUTED\n" : "Status: HAS NOT EXECUTED\n");
    return description.str();
}
