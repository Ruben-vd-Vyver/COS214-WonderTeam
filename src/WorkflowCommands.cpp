#include "WorkflowCommands.h"

namespace
{
    // Checks the number of fields (the command word counts as one) and that none are empty
    void requireArgs(const std::vector<std::string> &args, size_t minCount, size_t maxCount, const char *usage)
    {
        if (args.size() < minCount || args.size() > maxCount)
            throw std::string("usage: ") + usage;
        for (size_t i = 1; i < args.size(); ++i)
        {
            if (args[i].empty())
                throw std::string("empty field, usage: ") + usage;
        }
    }

    // Reads a positive whole number of up to nine digits; false for anything else
    bool parseVersion(const std::string &text, int &value)
    {
        if (text.empty() || text.size() > 9)
            return false;
        int result = 0;
        for (size_t i = 0; i < text.size(); ++i)
        {
            if (text[i] < '0' || text[i] > '9')
                return false;
            result = result * 10 + (text[i] - '0');
        }
        if (result < 1)
            return false;
        value = result;
        return true;
    }

    // ASCII lowercase copy, used for case-insensitive stage modes
    std::string lower(const std::string &str)
    {
        std::string result = str;
        for (size_t i = 0; i < result.size(); ++i)
        {
            if (result[i] >= 'A' && result[i] <= 'Z')
                result[i] = static_cast<char>(result[i] + ('a' - 'A'));
        }
        return result;
    }
}

// Starts the workflow definition; only one 'workflow' line is allowed per scenario
void WorkflowCommand::execute(WorkflowBuilder &builder, const std::vector<std::string> &args, DirectorContext &context)
{
    requireArgs(args, 3, 3, "workflow | <name> | <version>");
    if (context.started)
        throw std::string("only one 'workflow' line is allowed");

    int version = 0;
    if (!parseVersion(args[2], version))
        throw std::string("version must be a whole number of 1 or more");

    builder.reset(args[1], version);
    context.started = true;
    context.openStages = 0;
}

// Opens a stage; sequential unless the mode says parallel
void StageCommand::execute(WorkflowBuilder &builder, const std::vector<std::string> &args, DirectorContext &context)
{
    requireArgs(args, 2, 3, "stage | <name> [| parallel|sequential]");
    bool parallel = false;
    if (args.size() == 3)
    {
        std::string mode = lower(args[2]);
        if (mode != "parallel" && mode != "sequential")
            throw std::string("stage mode must be 'parallel' or 'sequential'");
        parallel = (mode == "parallel");
    }
    builder.beginStage(args[1], parallel);
    ++context.openStages;
}

// Closes the open stage
void EndCommand::execute(WorkflowBuilder &builder, const std::vector<std::string> &args, DirectorContext &context)
{
    requireArgs(args, 1, 1, "end");
    if (context.openStages == 0)
        throw std::string("'end' without an open stage");
    builder.endStage();
    --context.openStages;
}

// Adds a task for a role to the open stage
void TaskCommand::execute(WorkflowBuilder &builder, const std::vector<std::string> &args, DirectorContext &)
{
    requireArgs(args, 3, 3, "task | <name> | <role>");
    builder.addTask(args[1], args[2]);
}

// Attaches a named, registered approval chain to the task added just before
void ApproveCommand::execute(WorkflowBuilder &builder, const std::vector<std::string> &args, DirectorContext &context)
{
    requireArgs(args, 3, 3, "approve | <trigger> | <chain name>");
    std::map<std::string, ApprovalHandler *>::const_iterator it = context.chains.find(args[2]);
    if (it == context.chains.end())
        throw std::string("unknown approval chain '" + args[2] + "' (register it with registerApprovalChain first)");
    builder.requireApproval(args[1], it->second);
}

// Stops reading the scenario
void DoneCommand::execute(WorkflowBuilder &, const std::vector<std::string> &args, DirectorContext &context)
{
    requireArgs(args, 1, 1, "done");
    context.finished = true;
}
