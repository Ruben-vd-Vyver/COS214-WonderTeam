#ifndef AUDITLOG_H
#define AUDITLOG_H

#include <string>
#include <vector>
#include "WorkflowObserver.h"

//ConcreteObserver
class AuditLog : public WorkflowObserver {
    private:
        std::vector<std::string> entries;
        std::string formatTime(std::time_t time) const;
        std::string formatEvent( const WorkflowEvent& event) const;
    public:
        void onEvent(const WorkflowEvent& event);
        const std::vector<std::string>& getEntries() const;
        void printEntries() const;
};

#endif