#ifndef WORKFLOWEVENT_H
#define WORKFLOWEVENT_H

#include <ctime>
#include <string>

struct WorkflowEvent {
    std::string type;
    std::string sourceId;
    std::time_t timestamp;

    WorkflowEvent(const std::string& eventType, const std::string& eventSourceId)
                : type(eventType), sourceId(eventSourceId), timestamp(std::time(NULL)) {}
};

#endif