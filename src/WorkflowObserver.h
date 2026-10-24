#ifndef WORKFLOWOBSERVER_H
#define WORKFLOWOBSERVER_H

#include "WorkflowEvent.h"

//Observer
class WorkflowObserver{
    public:
    virtual ~WorkflowObserver() {}
    virtual void onEvent(const WorkflowEvent& event) = 0;
};

#endif