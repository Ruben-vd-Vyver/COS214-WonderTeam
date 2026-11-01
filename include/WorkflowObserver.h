#ifndef WORKFLOWOBSERVER_H
#define WORKFLOWOBSERVER_H

#include "WorkflowEvent.h"

//Observer
/**
 * @brief Represents the workflow observer abstraction in the TIVIDY workflow system.
 */
class WorkflowObserver{
    public:
        /**
         * @brief Destroys this object and releases owned resources.
         */
        virtual ~WorkflowObserver() {}
        /**
         * @brief Performs the on event operation.
         * @param event Value or object supplied by the caller.
         */
        virtual void onEvent(const WorkflowEvent& event) = 0;
};

#endif
