#ifndef COMPLETEDSTATE_H
#define COMPLETEDSTATE_H

#include "TaskState.h"

class CompletedState : public TaskState
{
public:
    std::string getName() const override;
    bool isComplete() const override;

    // [Prototype]
    CompletedState *clone() const override;
};

#endif // COMPLETEDSTATE_H
