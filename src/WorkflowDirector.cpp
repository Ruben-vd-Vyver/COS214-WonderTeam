#include "WorkflowDirector.h"
#include <istream>
#include <sstream>

// Initializes the director with an input stream and the default commands
WorkflowDirector::WorkflowDirector(std::istream &inputSource) : in_(inputSource)
{
    initializeDefaultCommands();
}

// Commands are owned by unique_ptr, so there is nothing to release by hand
WorkflowDirector::~WorkflowDirector() {}

// Removes space from both ends of string
std::string WorkflowDirector::trim(const std::string &str)
{
    size_t first = 0;
    while (first < str.size() && (str[first] == ' ' || str[first] == '\t' || str[first] == '\r' || str[first] == '\n'))
    {
        ++first;
    }
    size_t last = str.size();
    while (last > first && (str[last - 1] == ' ' || str[last - 1] == '\t' || str[last - 1] == '\r' || str[last - 1] == '\n'))
    {
        --last;
    }
    return str.substr(first, last - first);
}

// Converts to lowercase
std::string WorkflowDirector::toLower(const std::string &str)
{
    std::string result = str;
    for (size_t i = 0; i < result.size(); ++i)
    {
        if (result[i] >= 'A' && result[i] <= 'Z')
        {
            result[i] = static_cast<char>(result[i] + ('a' - 'A'));
        }
    }
    return result;
}

// Splits line into trimmed fields separated by the delimiter
std::vector<std::string> WorkflowDirector::tokenize(const std::string &line, char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string item;
    while (std::getline(ss, item, delimiter))
    {
        tokens.push_back(trim(item));
    }
    return tokens;
}

// Registers a named approval chain for 'approve' lines to refer to
void WorkflowDirector::registerApprovalChain(const std::string &name, ApprovalHandler *chain)
{
    if (chain == nullptr)
        throw std::string("WorkflowDirector::registerApprovalChain: chain is null");
    context_.chains[name] = chain;
}

// Creates one command object per command word
void WorkflowDirector::initializeDefaultCommands()
{
    commands_["workflow"].reset(new WorkflowCommand());
    commands_["stage"].reset(new StageCommand());
    commands_["end"].reset(new EndCommand());
    commands_["task"].reset(new TaskCommand());
    commands_["approve"].reset(new ApproveCommand());
    commands_["done"].reset(new DoneCommand());
}

// Parse input lines and execute builder commands to construct a workflow
void WorkflowDirector::construct(WorkflowBuilder &builder)
{
    context_.started = false;
    context_.finished = false;
    context_.openStages = 0;

    std::string line;
    int lineNum = 0;

    while (!context_.finished && std::getline(in_, line))
    {
        ++lineNum;
        std::string cleanedLine = trim(line);

        if (cleanedLine.empty() || cleanedLine[0] == '#')
            continue;

        try
        {
            std::vector<std::string> tokens = tokenize(cleanedLine, '|');
            std::string cmdName = toLower(tokens[0]);

            std::map<std::string, std::unique_ptr<Command>>::const_iterator it = commands_.find(cmdName);
            if (it == commands_.end())
                throw std::string("unknown command '" + cmdName + "'");

            it->second->execute(builder, tokens, context_);
        }
        catch (const std::string &e)
        {
            throw std::string("line " + std::to_string(lineNum) + ": " + e);
        }
    }

    if (!context_.started)
        throw std::string("scenario input has no 'workflow' line");
    if (context_.openStages > 0)
        throw std::string("input ended with " + std::to_string(context_.openStages) +
                          " stage(s) still open (missing 'end')");
}
