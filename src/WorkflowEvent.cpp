#include "WorkflowEvent.h"

WorkflowEvent::WorkflowEvent( const std::string& eventType, WorkItem* eventItem,
                            WorkflowInstance* eventInstance, const std::string& eventDetail,
                            std::time_t eventDeadline, bool eventHasDeadline)
                            : type(eventType), item(eventItem), instance(eventInstance),
                            detail(eventDetail),timestamp(std::time(NULL)),
                            deadline(eventDeadline), hasDeadline(eventHasDeadline) {}


std::string WorkflowEvent::getType() const {
    return type;
}

WorkItem* WorkflowEvent::getItem() const {
    return item;
}

WorkflowInstance* WorkflowEvent::getInstance() const {
    return instance;
}

std::string WorkflowEvent::getDetail() const {
    return detail;
}

std::time_t WorkflowEvent::getTimestamp() const {
    return timestamp;
}

std::time_t WorkflowEvent::getDeadline() const {
    return deadline;
}

bool WorkflowEvent::hasDeadlineValue() const {
    return hasDeadline;
}