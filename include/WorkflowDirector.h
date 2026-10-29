#ifndef WORKFLOWDIRECTOR_H
#define WORKFLOWDIRECTOR_H

#include "Director.h"

class ApprovalHandler;

class WorkflowDirector : public Director
{
private:
    ApprovalHandler *signOffChain_;
    ApprovalHandler *certChain_;

public:
    WorkflowDirector(ApprovalHandler *signOffChain = nullptr, ApprovalHandler *certChain = nullptr);
    void construct(WorkflowBuilder &b) override;
};

#endif
