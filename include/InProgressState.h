#ifndef INPROGRESSSTATE_H
#define INPROGRESSSTATE_H

#include "TaskState.h"

class InProgressState : public TaskState
{
public:
    std::string getName() const override;
    bool complete(Task &t) override;                          
    bool requestApproval(Task &t) override;                   
    bool reject(Task &t, const std::string &reason) override; 
    bool escalate(Task &t) override;                          

    // [Prototype]
    InProgressState *clone() const override;
};

#endif // INPROGRESSSTATE_H
