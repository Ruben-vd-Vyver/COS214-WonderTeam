#ifndef WORKFLOW_COMMANDS_H
#define WORKFLOW_COMMANDS_H

#include <map>
#include <string>
#include <vector>
#include "WorkflowBuilder.h"

class ApprovalHandler;

struct DirectorContext
{
    std::map<std::string, ApprovalHandler *> chains;
    bool started;
    bool finished;
    int openStages;

    DirectorContext() : started(false), finished(false), openStages(0) {}
};

class Command
{
public:
    virtual ~Command() {}

    virtual void execute(WorkflowBuilder &builder, const std::vector<std::string> &args, DirectorContext &context) = 0;
};

class WorkflowCommand : public Command
{
public:
    void execute(WorkflowBuilder &builder, const std::vector<std::string> &args, DirectorContext &context) override;
};

class StageCommand : public Command
{
public:
    void execute(WorkflowBuilder &builder, const std::vector<std::string> &args, DirectorContext &context) override;
};

class EndCommand : public Command
{
public:
    void execute(WorkflowBuilder &builder, const std::vector<std::string> &args, DirectorContext &context) override;
};

class TaskCommand : public Command
{
public:
    void execute(WorkflowBuilder &builder, const std::vector<std::string> &args, DirectorContext &context) override;
};

class ApproveCommand : public Command
{
public:
    void execute(WorkflowBuilder &builder, const std::vector<std::string> &args, DirectorContext &context) override;
};

class DoneCommand : public Command
{
public:
    void execute(WorkflowBuilder &builder, const std::vector<std::string> &args, DirectorContext &context) override;
};

#endif // WORKFLOW_COMMANDS_H
