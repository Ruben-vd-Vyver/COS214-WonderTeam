#ifndef DEADLINEMONITOR_H
#define DEADLINEMONITOR_H

#include <ctime>
#include <map>

#include "WorkflowObserver.h"

/**
 * @brief Represents the deadline monitor abstraction in the TIVIDY workflow system.
 */
class DeadlineMonitor : public WorkflowObserver {
    private:
        std::map<WorkItem*, std::time_t> deadlines;

    public:
        /**
         * @brief Performs the on event operation.
         * @param event Value or object supplied by the caller.
         */
        void onEvent(const WorkflowEvent& event);
        /**
         * @brief Performs the check overdue operation.
         * @param now Value or object supplied by the caller.
         */
        void checkOverdue(std::time_t now) const;
};

#endif