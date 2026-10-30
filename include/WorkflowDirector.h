#ifndef WORKFLOW_DIRECTOR_H
#define WORKFLOW_DIRECTOR_H

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "Director.h"
#include "WorkflowCommands.h"

class ApprovalHandler;

class WorkflowDirector : public Director
{
public:
    explicit WorkflowDirector(std::istream &inputSource);
    ~WorkflowDirector();
    WorkflowDirector(const WorkflowDirector &) = delete;
    WorkflowDirector &operator=(const WorkflowDirector &) = delete;
    void construct(WorkflowBuilder &builder) override;
    void registerApprovalChain(const std::string &name, ApprovalHandler *chain);

private:
    std::istream &in_;
    DirectorContext context_;
    std::map<std::string, std::unique_ptr<Command>> commands_;

    void initializeDefaultCommands();
    std::vector<std::string> tokenize(const std::string &line, char delimiter);
    std::string trim(const std::string &str);
    std::string toLower(const std::string &str);
};

#endif // WORKFLOW_DIRECTOR_H
