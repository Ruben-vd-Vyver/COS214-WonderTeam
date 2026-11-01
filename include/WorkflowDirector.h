#ifndef WORKFLOW_DIRECTOR_H
#define WORKFLOW_DIRECTOR_H

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "Director.h"
#include "WorkflowCommands.h"

/**
 * @brief Represents the approval handler abstraction in the TIVIDY workflow system.
 */
class ApprovalHandler;

/**
 * @brief Represents the workflow director abstraction in the TIVIDY workflow system.
 */
class WorkflowDirector : public Director
{
public:
    /**
     * @brief Constructs an object of this type.
     * @param inputSource Value or object supplied by the caller.
     */
    explicit WorkflowDirector(std::istream &inputSource);
    /**
     * @brief Destroys this object and releases owned resources.
     */
    ~WorkflowDirector();
    /**
     * @brief Constructs an object of this type.
     */
    WorkflowDirector(const WorkflowDirector &) = delete;
    WorkflowDirector &operator=(const WorkflowDirector &) = delete;

    /**
     * @brief Performs the construct operation.
     * @param builder Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    bool construct(WorkflowBuilder &builder) override;
    /**
     * @brief Registers the supplied object with this component.
     * @param name Value or object supplied by the caller.
     * @param chain Value or object supplied by the caller.
     */
    void registerApprovalChain(const std::string &name, ApprovalHandler *chain);
    /**
     * @brief Returns the error.
     * @return Result produced by the operation.
     */
    const std::string &getError() const; 

private:
    std::istream &in_;
    DirectorContext context_;
    std::map<std::string, std::unique_ptr<Command>> commands_;
    std::string error_;

    /**
     * @brief Performs the initialize default commands operation.
     */
    void initializeDefaultCommands();
    /**
     * @brief Performs the tokenize operation.
     * @param line Value or object supplied by the caller.
     * @param delimiter Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    std::vector<std::string> tokenize(const std::string &line, char delimiter);
    /**
     * @brief Performs the trim operation.
     * @param str Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    std::string trim(const std::string &str);
    /**
     * @brief Performs the to lower operation.
     * @param str Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    std::string toLower(const std::string &str);
};

#endif // WORKFLOW_DIRECTOR_H
