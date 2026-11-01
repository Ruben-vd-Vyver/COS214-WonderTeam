#ifndef NOTIFICATIONSERVICE_H
#define NOTIFICATIONSERVICE_H

#include "WorkflowObserver.h"

//ConcreteObserver
class NotificationService : public WorkflowObserver {
    public:
        void onEvent(const WorkflowEvent& event);
};

#endif