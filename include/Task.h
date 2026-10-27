#ifndef TASK_H
#define TASK_H

#include <memory>
#include "WorkflowNode.h"
#include "TaskState.h"

class Participant; // forward declaration (Participant.h includes Task.h)

class Task : public WorkflowNode
{
    friend class TaskState;

protected:
    std::unique_ptr<TaskState> state_;
    bool activated_ = false;
    Participant *assignee_ = nullptr;
    std::string description_;

public:
    Task(std::string id, std::string name, std::string description = "");
    virtual bool assign(Participant *p);
    virtual bool start();
    virtual bool complete();
    virtual bool requestApproval();
    virtual bool reject(const std::string &reason);
    virtual bool escalate();
    virtual std::string getState() const;
    virtual Participant *getAssignee() const;
    virtual int getPriority() const;
    virtual std::string describe() const;
    void activate() override;
    bool isComplete() const override;
    bool hasFailed() const override;
    void print(int indent = 0) const override;

};

#endif
