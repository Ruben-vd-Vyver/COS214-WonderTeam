#ifndef WORKFLOW_DIRECTOR_H
#define WORKFLOW_DIRECTOR_H

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include "WorkflowBuilder.h"
#include "ApprovalHandler.h"
#include "WorkflowCommands.h"

class WorkflowDirector
{
public:
    explicit WorkflowDirector(std::istream &inputSource);
    virtual ~WorkflowDirector();

    bool construct(WorkflowBuilder &builder);
    void registerApprovalChain(const std::string &name, ApprovalHandler *chain);

private:
    std::istream &in_;
    std::map<std::string, ApprovalHandler *> chains_;
    std::map<std::string, Command *> commands_;

    void initializeDefaultCommands();
    std::vector<std::string> tokenize(const std::string &line, char delimiter);
    std::string trim(const std::string &str);
    std::string toLower(const std::string &str);
};

#endif // WORKFLOW_DIRECTOR_H