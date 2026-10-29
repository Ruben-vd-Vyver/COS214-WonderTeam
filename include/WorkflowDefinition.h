#ifndef WORKFLOWDEFINITION_H
#define WORKFLOWDEFINITION_H

#include <memory>
#include <string>
#include <vector>

class WorkflowNode;
class WorkflowInstance;
class ApprovalHandler;
class Task;

class WorkflowDefinition
{
private:
    std::string name_;
    int version_;
    std::shared_ptr<WorkflowNode> root_;
    std::vector<std::shared_ptr<Task>> tasks_;
    std::vector<ApprovalHandler *> chains_;

public:
    WorkflowDefinition(std::string name, int version);
    void setRoot(std::shared_ptr<WorkflowNode> root);
    std::shared_ptr<WorkflowNode> getRoot() const;
    const std::string &getName() const;
    int getVersion() const;
    void addTask(std::shared_ptr<Task> t);
    const std::vector<std::shared_ptr<Task>> &getTasks() const;
    void addChain(ApprovalHandler *h);
    const std::vector<ApprovalHandler *> &getChains() const;
    WorkflowInstance *createInstance() const;
};

#endif
