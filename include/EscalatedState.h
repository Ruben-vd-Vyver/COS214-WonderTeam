#ifndef ESCALATEDSTATE_H
#define ESCALATEDSTATE_H

#include "TaskState.h"

class EscalatedState : public TaskState
{
public:
    std::string getName() const override;
    bool assign(Task &t, Participant *p) override;

    // [Prototype]
    EscalatedState *clone() const override;
};

#endif // ESCALATEDSTATE_H
