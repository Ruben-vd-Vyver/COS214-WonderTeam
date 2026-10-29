#include "WorkflowDirector.h"
#include <sstream>

// Initializes the director with an is and default command
WorkflowDirector::WorkflowDirector(std::istream &inputSource) : in_(inputSource)
{
    initializeDefaultCommands();
}

// Releases command objects owned by the director
WorkflowDirector::~WorkflowDirector()
{
    for (auto &pair : commands_)
    {
        delete pair.second;
    }
}

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
            result[i] = result[i] + ('a' - 'A');
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

// Registers a named approval chain for later
void WorkflowDirector::registerApprovalChain(const std::string &name, ApprovalHandler *chain)
{
    if (chain != nullptr)
    {
        chains_[name] = chain;
    }
}

// Creates the stage and task commands
void WorkflowDirector::initializeDefaultCommands()
{
    commands_["stage"] = new StageCommand();
    commands_["task"] = new TaskCommand();
}

// Parse input lines and execute builder commands to construct a workflow
bool WorkflowDirector::construct(WorkflowBuilder &builder)
{
    std::string line;
    int lineNum = 0;

    while (std::getline(in_, line))
    {
        ++lineNum;
        std::string cleanedLine = trim(line);

        if (cleanedLine.empty() || cleanedLine[0] == '#')
            continue;

        std::vector<std::string> tokens = tokenize(cleanedLine, '|');
        if (tokens.empty())
            continue;

        std::string cmdName = toLower(tokens[0]);

        auto it = commands_.find(cmdName);
        if (it != commands_.end())
        {
            if (!it->second->execute(builder, tokens))
            {
                std::cerr << "Failed at line " << lineNum << "\n";
                return false;
            }
        }
        else
        {
            std::cerr << "Error: Unknown command '" << cmdName << "' at line " << lineNum << "\n";
            return false;
        }
    }
    return true;
}