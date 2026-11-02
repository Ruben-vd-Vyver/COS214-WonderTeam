#include "AuditLog.h"

#include <iostream>
#include <sstream>

//adds an event to entries
void AuditLog::onEvent(const WorkflowEvent& event) {
    entries.push_back(formatEvent(event));
}

const std::vector<std::string>& AuditLog::getEntries() const {
    return entries;
}

void AuditLog::printEntries() const {
    for (std::vector<std::string>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
        std::cout << *it << std::endl;
    }
}

std::string AuditLog::formatTime(std::time_t time) const {
    std::string result;
    const char* timeText = std::ctime(&time);

    if (timeText != NULL) {
        result = timeText;
        //remove ctime newline
        if (!result.empty() && result[result.size() - 1] == '\n') {
            result.erase(result.size() - 1);
        }
    }

    return result;
}

std::string AuditLog::formatEvent(const WorkflowEvent& event) const {
    std::ostringstream output;

    output << "Event: " << event.getType()
           << " | Detail: " << event.getDetail()
           << " | Occurred: "
           << formatTime(event.getTimestamp());

    if (event.hasDeadlineValue()) {
        output << " | Deadline: "
               << formatTime(event.getDeadline());
    }

    return output.str();
}