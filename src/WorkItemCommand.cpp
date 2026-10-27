#include "../include/WorkItemCommand.h"

WorkItemCommand::WorkItemCommand(Task* task, Participant* participant) 
: task_(task), participant_(participant) {
    // Basic constructor
    hasExecuted_ = false;
}