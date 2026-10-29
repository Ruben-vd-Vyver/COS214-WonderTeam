#ifndef BASICWORKITEM_H
#define BASICWORKITEM_H

#include <map>
#include <memory>
#include <string>
#include "WorkItem.h"
#include "WorkItemState.h"

class Participant;
class Task;

class BasicWorkItem : public WorkItem
{
    friend class WorkItemState;

private:
    std::shared_ptr<const Task> task_;
    std::map<std::string, std::string> fields_;
    std::unique_ptr<WorkItemState> state_;
    Participant *assignee_;
    bool activated_;
    static const Task &requireSpec(const std::shared_ptr<const Task> &spec);

protected:
    virtual void publish(const std::string &type);

public:
    BasicWorkItem(std::string id, std::shared_ptr<const Task> spec);
    bool assign(Participant *p) override;
    bool start() override;
    bool complete() override;
    bool requestApproval() override;
    bool reject(const std::string &reason) override;
    bool escalate() override;
    void activate() override;
    bool isComplete() const override;
    bool hasFailed() const override;
    WorkflowNode *clone() const override;
    void setState(WorkItemState *s);
    std::string getStateName() const override;
    const Task &getTask() const override;
    Participant *getAssignee() const;
    void setField(const std::string &key, const std::string &value);
    std::string getField(const std::string &key) const;
    void print(int indent = 0) const override;
};

#endif
