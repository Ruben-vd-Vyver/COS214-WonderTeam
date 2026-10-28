#ifndef WORKFLOWSUBJECT_H
#define WORKFLOWSUBJECT_H

#include <vector>
#include <queue>

#include "WorkflowObserver.h"

//Subject
class WorkflowSubject {
    private:
        std::vector<WorkflowObserver*> observers; 
        std::queue<WorkflowEvent> pendingEvents;
        bool dispatching; //flag
    public:
        WorkflowSubject();
        virtual ~WorkflowSubject();
        void attach(WorkflowObserver* observer);
        void detach(WorkflowObserver* observer);
        void notify(const WorkflowEvent& event);

};

#endif