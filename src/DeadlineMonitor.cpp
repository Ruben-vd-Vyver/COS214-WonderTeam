#include "DeadlineMonitor.h"

#include <iostream>

void DeadlineMonitor::onEvent(const WorkflowEvent& event) {
    WorkItem* item = event.getItem();

    if (item == NULL) {
        return;
    }

    if (event.getType() == "DeadlineSet") {
        if (!event.hasDeadlineValue()) {
            return;
        }

        deadlines[item] = event.getDeadline();

        std::cout<< "[DeadlineMonitor] Deadline registered"<< std::endl;
    }
    else if (event.getType() == "WorkItemCompleted" || event.getType() == "ApprovalRejected") {
        deadlines.erase(item);
    }
}

void DeadlineMonitor::checkOverdue(std::time_t now) const {
    for (std::map<WorkItem*, std::time_t>::const_iterator it = deadlines.begin(); it != deadlines.end(); ++it) {
        if (now > it->second) {
            std::cout << "[DeadlineMonitor] A work item is overdue"<< std::endl;
        }
    }
}