#ifndef TASK_H
#define TASK_H

#include <map>
#include <string>
#include <vector>

/**
 * @brief Represents the approval handler abstraction in the TIVIDY workflow system.
 */
class ApprovalHandler;

/**
 * @brief Represents the task abstraction in the TIVIDY workflow system.
 */
class Task
{
private:
    std::string name_;
    std::string requiredRole_;
    std::vector<std::string> requiredFields_;
	struct ApprovalInfo {
            ApprovalHandler* chain;
            int requiredLevel;
        };
    std::map<std::string, ApprovalInfo> approvals_;

public:
    /**
     * @brief Constructs an object of this type.
     * @param name Value or object supplied by the caller.
     * @param role Value or object supplied by the caller.
     */
    Task(std::string name, std::string role);
    /**
     * @brief Returns the name.
     * @return Result produced by the operation.
     */
    const std::string &getName() const;
    /**
     * @brief Returns the required role.
     * @return Result produced by the operation.
     */
    const std::string &getRequiredRole() const;
    /**
     * @brief Adds the required field.
     * @param field Value or object supplied by the caller.
     */
    void addRequiredField(const std::string &field);
    /**
     * @brief Returns the required fields.
     * @return Result produced by the operation.
     */
    const std::vector<std::string> &getRequiredFields() const;
    /**
     * @brief Performs the require approval operation.
     * @param trigger Value or object supplied by the caller.
     * @param chain Value or object supplied by the caller.
     */
    void requireApproval(const std::string &trigger, ApprovalHandler *chain);
    /**
     * @brief Performs the requires approval operation.
     * @param trigger Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    bool requiresApproval(const std::string &trigger) const;
    /**
     * @brief Returns the approval chain.
     * @param trigger Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    ApprovalHandler *getApprovalChain(const std::string &trigger) const;
	int getRequiredLevel(const std::string& trigger) const;
};

#endif
