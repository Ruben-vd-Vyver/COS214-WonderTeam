#include "WorkflowSubject.h"

#include <algorithm>

WorkflowSubject::WorkflowSubject() {}

WorkflowSubject::WorkflowSubject(): dispatching(false) {}

WorkflowSubject::~WorkflowSubject() {
    while (!pendingEvents.empty()) {
        pendingEvents.pop();
    } //clears queue

    //Observers not owned by subject
    observers.clear();
}

void WorkflowSubject::attach(WorkflowObserver* observer) {
    if (observer == NULL) {
        return;
    }

    if (std::find(observers.begin(), observers.end(), observer) == observers.end()) {
        observers.push_back(observer); //if not in list then adds
    }
}

void WorkflowSubject::detach(WorkflowObserver* observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer),
                    observers.end()
    );
}

//broadcast workflow events to all registered observers
//push observer keeps observers and inner workings of WorkflowSubject decoupled
void WorkflowSubject::notify(const WorkflowEvent& event) { 
    pendingEvents.push(event);

    if (dispatching) {
        return; //loop already running
    }

    dispatching = true;

    while (!pendingEvents.empty()) {
        WorkflowEvent currentEvent = pendingEvents.front(); //processing events FIFO order
        pendingEvents.pop();

        //so not invalidated when observer registration changes
        std::vector<WorkflowObserver*> currentObservers(
            observers
        );

        for (std::vector<WorkflowObserver*>::iterator it = currentObservers.begin();
             it != currentObservers.end(); ++it) {

            //skip is detached
            if (*it != NULL && std::find(observers.begin(), observers.end(), *it) != observers.end()) {
                (*it)->onEvent(currentEvent);
            }
        }
    }
    dispatching = false;
}