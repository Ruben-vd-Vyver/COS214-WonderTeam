#include "../include/AssignCommand.h"
#include "../include/Participant.h"
#include <sstream>
#include <stdexcept>

AssignCommand::AssignCommand(WorkItem* workItem, Participant* participant)
    : WorkItemCommand(workItem, participant) {
    if (!participant_) {
        throw std::invalid_argument("an assignment command needs a Participant");
    }
}

bool AssignCommand::execute(){
    if (hasExecuted_)
        return false;

    previousAssignee_ = workItem_->getAssignee();
    if (previousAssignee_ == participant_)
        return false;

    workItem_->assign(participant_);
    if (workItem_->getAssignee() != participant_)
        return false;

    hasExecuted_ = true;
    std::cout << "Command executed successfully\n";
    return true;
}

bool AssignCommand::undo(){
    if (!hasExecuted_ || workItem_->getAssignee() != participant_)
        return false;

    workItem_->assign(previousAssignee_);
    if (workItem_->getAssignee() != previousAssignee_)
        return false;

    hasExecuted_ = false;
    std::cout << "Command reverted successfully\n";
    return true;
}

std::string AssignCommand::describe(){
    std::stringstream description;
    description << "Assignment Command\nInvolved WorkItem:\n"
                << workItem_->getId() << " - " << workItem_->getName()
                << "\nInvolved participant\n" << participant_->describe() << "\n";
    description << ((hasExecuted_)? "Status: HAS EXECUTED\n" : "Status: HAS NOT EXECUTED\n");
    return description.str();
}