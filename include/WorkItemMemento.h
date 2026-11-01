#ifndef WORKITEMMEMENTO_H
#define WORKITEMMEMENTO_H

#include <map>
#include <memory>
#include <string>

class WorkItemState;
class Participant;
class BasicWorkItem;

class WorkItemMemento {
    friend class BasicWorkItem;

private:
    std::unique_ptr<WorkItemState> savedState_;
    Participant* assignee_;
    std::map<std::string, std::string> payload_;

    WorkItemMemento(WorkItemState* state, Participant* assignee,
                    std::map<std::string, std::string> data);

public:
    ~WorkItemMemento();
    WorkItemMemento(const WorkItemMemento&) = delete;
    WorkItemMemento& operator=(const WorkItemMemento&) = delete;
    WorkItemMemento(WorkItemMemento&&) = default;
    WorkItemMemento& operator=(WorkItemMemento&&) = default;
};

#endif
