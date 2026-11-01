#ifndef NOTIFICATIONSERVICE_H
#define NOTIFICATIONSERVICE_H

#include "WorkflowObserver.h"

//ConcreteObserver
/**
 * @brief Represents the notification service abstraction in the TIVIDY workflow system.
 */
class NotificationService : public WorkflowObserver {
    public:
        /**
         * @brief Performs the on event operation.
         * @param event Value or object supplied by the caller.
         */
        void onEvent(const WorkflowEvent& event);
};

#endif
