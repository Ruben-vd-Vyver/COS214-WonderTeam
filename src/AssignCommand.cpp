#include "../include/AssignCommand.h"
#include <sstream>

AssignCommand::AssignCommand(Task* task, Participant* participant) : WorkItemCommand(task,participant)  {
    //Basic constructor, just calls Parents constructor
}

bool AssignCommand::execute(){
    if (hasExecuted_)
        return false;
    if (task_->assign(participant_)){
        std::cout << "Command executed successfully\n";
        hasExecuted_ = true;
        return true;
    }
    std::cout << "Error when executing command: Task in a fixed/final state\n";
    return false;
}

bool AssignCommand::undo(){
    if (!hasExecuted_)
        return false;
    if (task_->unassign(participant_)){
        std::cout << "Command revert successfully\n";
        hasExecuted_ = false;
        return true;
    }
    std::cout << "Error when reverting command: Task in a fixed/final state\n";
    return false;
}

std::string AssignCommand::describe(){
    std::stringstream description;
    description << "Assignment Command\nInvolved Task:\n" << task_->describe() << "\nInvolved participant\n" << participant_->describe() << "\n";
    description << ((hasExecuted_)? "Status: HAS EXECUTED\n" : "Status: HAS NOT EXECUTED\n");
    return description.str();
}