#include "NotificationService.h"

#include <iostream>

void NotificationService::onEvent(const WorkflowEvent& event) {
    std::cout << "[Notification] "
              << event.getType()
              << ": "
              << event.getDetail()
              << std::endl;
}