#include "../include/WorkItemCommand.h"
#include <stdexcept>

WorkItemCommand::WorkItemCommand(WorkItem* workItem, Participant* participant)
: workItem_(workItem), participant_(participant) {
    if (!workItem_) {
        throw std::invalid_argument("a work item command needs a WorkItem");
    }
}