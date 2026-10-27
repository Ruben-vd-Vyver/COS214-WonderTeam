#ifndef TASKSTATE_H
#define TASKSTATE_H

#include <string>

class Task;
class Participant;
// State pattern - abstract State

class TaskState
{
public:
    virtual ~TaskState() {}

    virtual std::string getName() const = 0;

    virtual void activate(Task &t);
    virtual bool assign(Task &t, Participant *p);
    virtual bool start(Task &t);
    virtual bool complete(Task &t);
    virtual bool requestApproval(Task &t);
    virtual bool reject(Task &t, const std::string &reason);
    virtual bool escalate(Task &t);

    virtual bool isComplete() const; // true only in CompletedState
    virtual bool hasFailed() const;  // true only in RejectedState

protected:
    static void changeState(Task &t, TaskState *next); // will use unique ptr
    static void setAssignee(Task &t, Participant *p);
    static void markActivated(Task &t);
};

#endif // TASKSTATE_H
