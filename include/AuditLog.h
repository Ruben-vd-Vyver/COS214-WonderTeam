#ifndef AUDITLOG_H
#define AUDITLOG_H

#include <string>
#include <vector>
#include "WorkflowObserver.h"

//ConcreteObserver
/**
 * @brief Represents the audit log abstraction in the TIVIDY workflow system.
 */
class AuditLog : public WorkflowObserver {
    private:
        std::vector<std::string> entries;
        /**
         * @brief Performs the format time operation.
         * @param time Value or object supplied by the caller.
         * @return Result produced by the operation.
         */
        std::string formatTime(std::time_t time) const;
        /**
         * @brief Performs the format event operation.
         * @param event Value or object supplied by the caller.
         * @return Result produced by the operation.
         */
        std::string formatEvent( const WorkflowEvent& event) const;
    public:
        /**
         * @brief Performs the on event operation.
         * @param event Value or object supplied by the caller.
         */
        void onEvent(const WorkflowEvent& event);
        /**
         * @brief Returns the entries.
         * @return Result produced by the operation.
         */
        const std::vector<std::string>& getEntries() const;
        /**
         * @brief Performs the print entries operation.
         */
        void printEntries() const;
};

#endif
