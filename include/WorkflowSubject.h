#ifndef WORKFLOWSUBJECT_H
#define WORKFLOWSUBJECT_H

#include <vector>
#include <queue>

#include "WorkflowObserver.h"

//Subject
/**
 * @brief Represents the workflow subject abstraction in the TIVIDY workflow system.
 */
class WorkflowSubject {
    private:
        std::vector<WorkflowObserver*> observers; 
        std::queue<WorkflowEvent> pendingEvents;
        bool dispatching; //flag
    public:
        /**
         * @brief Constructs an object of this type.
         */
        WorkflowSubject();
        /**
         * @brief Destroys this object and releases owned resources.
         */
        virtual ~WorkflowSubject();
        /**
         * @brief Performs the attach operation.
         * @param observer Value or object supplied by the caller.
         */
        void attach(WorkflowObserver* observer);
        /**
         * @brief Performs the detach operation.
         * @param observer Value or object supplied by the caller.
         */
        void detach(WorkflowObserver* observer);
        /**
         * @brief Performs the notify operation.
         * @param event Value or object supplied by the caller.
         */
        void notify(const WorkflowEvent& event);

};

#endif