#ifndef AVAILABLESTATE_H
#define AVAILABLESTATE_H

#include "TaskState.h"

class AvailableState : public TaskState
{
public:
    std::string getName() const override;
    bool assign(Task &t, Participant *p) override; // -> Assigned
};

#endif // AVAILABLESTATE_H