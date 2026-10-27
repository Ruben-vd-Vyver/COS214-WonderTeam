#ifndef TASK_H
#define TASK_H

#include <memory>
#include "WorkflowNode.h"
#include "TaskState.h"

class Participant; //forward declaration (Participant.h includes Task.h)

class Task : public WorkflowNode
{
    friend class TaskState;

protected:
    std::unique_ptr<TaskState> state_;
    bool activated_ = false;
    Participant *assignee_ = nullptr;
    std::string description_;

    // [Prototype] deep copy: the current state object is cloned too, so the copy and the original never share a TaskState.
    // The assignee is a shared Participant (not owned), so the pointer is copied as-is.
    Task(const Task &other);

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

    // [Prototype] covariant return: cloning a Task gives back a Task*
    Task *clone() const override;
};

#endif
