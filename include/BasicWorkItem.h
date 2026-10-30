#ifndef BASICWORKITEM_H
#define BASICWORKITEM_H

#include <map>
#include <memory>
#include <string>
#include "WorkItem.h"
#include "WorkItemState.h"

class Participant;
class WorkflowSubject;
class BasicWorkItem : public WorkItem
{
private:
    std::shared_ptr<const Task> task_;
    std::map<std::string, std::string> payload_;
    std::unique_ptr<WorkItemState> state_;
    Participant *assignee_;
    WorkflowSubject *subject_;
    static const Task &requireSpec(const std::shared_ptr<const Task> &spec);

protected:
    virtual void publish(const std::string &type);

public:
    BasicWorkItem(std::string id, std::shared_ptr<const Task> spec);
    ~BasicWorkItem();

    void assign(Participant *p) override;
    void start() override;
    void complete() override;
    void reject(const std::string &reason) override;
    void escalate() override;
    void approve() override;

    void activate() override;
    bool isComplete() const override;
    void bind(WorkflowSubject *s) override;
    BasicWorkItem *clone() const override;

    bool isActive() const override;
    const Task &getTask() const override;
    std::string getRequiredRole() const override;
    std::string getStateName() const override;
    Participant *getAssignee() const override;
    void setField(const std::string &key, const std::string &value) override;
    std::string getField(const std::string &key) const override;

    void setState(WorkItemState *s);
    void setAssignee(Participant *p);
    void advanceTo(WorkItemState *next, const std::string &trigger);

    WorkItemMemento *createMemento() const override;
    void restore(const WorkItemMemento &m) override;
};

#endif // BASICWORKITEM_H
