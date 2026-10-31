#ifndef WORKFLOW_COMMANDS_H
#define WORKFLOW_COMMANDS_H

#include <vector>
#include <string>
#include <iostream>
#include "WorkflowBuilder.h"

// Abstract Base Class Command Pattern
class Command
{
public:
    virtual ~Command() = default;
    virtual bool execute(WorkflowBuilder &builder, const std::vector<std::string> &args) = 0;
};

// Concrete Command
class StageCommand : public Command
{
public:
    bool execute(WorkflowBuilder &builder, const std::vector<std::string> &args) override
    {
        if (args.size() < 2)
        {
            std::cerr << "Error: Usage is 'stage | <name> [| parallel|sequential]'\n";
            return false;
        }
        bool isParallel = (args.size() >= 3 && args[2] == "parallel");
        builder.beginStage(args[1], isParallel);
        return true;
    }
};

// Concrete Command
class TaskCommand : public Command
{
public:
    bool execute(WorkflowBuilder &builder, const std::vector<std::string> &args) override
    {
        if (args.size() < 3)
        {
            std::cerr << "Error: Usage is 'task | <name> | <role>'\n";
            return false;
        }
        builder.addTask(args[1], args[2]);
        return true;
    }
};

#endif // WORKFLOW_COMMANDS_H