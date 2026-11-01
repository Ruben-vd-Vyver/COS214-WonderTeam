#ifndef DEADLINEMONITOR_H
#define DEADLINEMONITOR_H

#include <ctime>
#include <map>

#include "WorkflowObserver.h"

class DeadlineMonitor : public WorkflowObserver {
    private:
        std::map<WorkItem*, std::time_t> deadlines;

    public:
        void onEvent(const WorkflowEvent& event);
        void checkOverdue(std::time_t now) const;
};

#endif