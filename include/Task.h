#ifndef TASK_H
#define TASK_H

#include <map>
#include <string>
#include <vector>

class ApprovalHandler;

class Task
{
private:
    std::string name_;
    std::string requiredRole_;
    std::vector<std::string> requiredFields_;
    std::map<std::string, ApprovalHandler *> approvals_;

public:
    Task(std::string name, std::string role);
    const std::string &getName() const;
    const std::string &getRequiredRole() const;
    void addRequiredField(const std::string &field);
    const std::vector<std::string> &getRequiredFields() const;
    void requireApproval(const std::string &trigger, ApprovalHandler *chain);
    bool requiresApproval(const std::string &trigger) const;
    ApprovalHandler *getApprovalChain(const std::string &trigger) const;
};

#endif
