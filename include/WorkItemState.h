#ifndef WORKITEMSTATE_H
#define WORKITEMSTATE_H

#include <string>

class BasicWorkItem;
class Participant;
class WorkItemState
{
public:
    virtual ~WorkItemState() {}

    virtual std::string getName() const = 0;
    virtual WorkItemState *clone() const = 0;

    virtual void activate(BasicWorkItem &item);
    virtual void assign(BasicWorkItem &item, Participant *p);
    virtual void start(BasicWorkItem &item);
    virtual void complete(BasicWorkItem &item);
    virtual void reject(BasicWorkItem &item, const std::string &reason);
    virtual void escalate(BasicWorkItem &item);
    virtual void approve(BasicWorkItem &item);

    virtual bool isActive() const;
    virtual bool isComplete() const;

protected:
    void illegal(const std::string &action) const;
};

#endif // WORKITEMSTATE_H
