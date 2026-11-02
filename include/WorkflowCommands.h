#ifndef WORKFLOW_COMMANDS_H
#define WORKFLOW_COMMANDS_H

#include <map>
#include <string>
#include <vector>
#include "WorkflowBuilder.h"

/**
 * @brief Represents the approval handler abstraction in the TIVIDY workflow system.
 */
class ApprovalHandler;

/**
 * @brief Represents the director context abstraction in the TIVIDY workflow system.
 */
struct DirectorContext
{
    std::map<std::string, ApprovalHandler *> chains;
    bool started;
    bool finished;
    int openStages;

    /**
     * @brief Constructs an object of this type.
     */
    DirectorContext() : started(false), finished(false), openStages(0) {}
};

/**
 * @brief Represents the command abstraction in the TIVIDY workflow system.
 */
class Command
{
public:
    /**
     * @brief Destroys this object and releases owned resources.
     */
    virtual ~Command() {}

    /**
     * @brief Performs the execute operation.
     * @param builder Value or object supplied by the caller.
     * @param args Value or object supplied by the caller.
     * @param context Value or object supplied by the caller.
     */
    virtual void execute(WorkflowBuilder &builder, const std::vector<std::string> &args, DirectorContext &context) = 0;
};

/**
 * @brief Represents the workflow command abstraction in the TIVIDY workflow system.
 */
class WorkflowCommand : public Command
{
public:
    /**
     * @brief Performs the execute operation.
     * @param builder Value or object supplied by the caller.
     * @param args Value or object supplied by the caller.
     * @param context Value or object supplied by the caller.
     */
    void execute(WorkflowBuilder &builder, const std::vector<std::string> &args, DirectorContext &context) override;
};

/**
 * @brief Represents the stage command abstraction in the TIVIDY workflow system.
 */
class StageCommand : public Command
{
public:
    /**
     * @brief Performs the execute operation.
     * @param builder Value or object supplied by the caller.
     * @param args Value or object supplied by the caller.
     * @param context Value or object supplied by the caller.
     */
    void execute(WorkflowBuilder &builder, const std::vector<std::string> &args, DirectorContext &context) override;
};

/**
 * @brief Represents the end command abstraction in the TIVIDY workflow system.
 */
class EndCommand : public Command
{
public:
    /**
     * @brief Performs the execute operation.
     * @param builder Value or object supplied by the caller.
     * @param args Value or object supplied by the caller.
     * @param context Value or object supplied by the caller.
     */
    void execute(WorkflowBuilder &builder, const std::vector<std::string> &args, DirectorContext &context) override;
};

/**
 * @brief Represents the task command abstraction in the TIVIDY workflow system.
 */
class TaskCommand : public Command
{
public:
    /**
     * @brief Performs the execute operation.
     * @param builder Value or object supplied by the caller.
     * @param args Value or object supplied by the caller.
     * @param context Value or object supplied by the caller.
     */
    void execute(WorkflowBuilder &builder, const std::vector<std::string> &args, DirectorContext &context) override;
};

/**
 * @brief Represents the approve line command abstraction in the TIVIDY workflow system.
 */
class ApproveLineCommand : public Command
{
public:
    /**
     * @brief Performs the execute operation.
     * @param builder Value or object supplied by the caller.
     * @param args Value or object supplied by the caller.
     * @param context Value or object supplied by the caller.
     */
    void execute(WorkflowBuilder &builder, const std::vector<std::string> &args, DirectorContext &context) override;
};

/**
 * @brief Represents the done command abstraction in the TIVIDY workflow system.
 */
class DoneCommand : public Command
{
public:
    /**
     * @brief Performs the execute operation.
     * @param builder Value or object supplied by the caller.
     * @param args Value or object supplied by the caller.
     * @param context Value or object supplied by the caller.
     */
    void execute(WorkflowBuilder &builder, const std::vector<std::string> &args, DirectorContext &context) override;
};

#endif // WORKFLOW_COMMANDS_H
