#ifndef WORKFLOWBUILDER_H
#define WORKFLOWBUILDER_H

#include <memory>
#include <string>

class WorkItem;
class ApprovalHandler;
class WorkflowDefinition;

class WorkflowBuilder
{
public:
    virtual ~WorkflowBuilder() {}
    virtual void reset(const std::string &name, int version) = 0;
    virtual void beginStage(const std::string &name, bool parallel) = 0;
    virtual void endStage() = 0;
    virtual void addTask(const std::string &name, const std::string &role) = 0;
    virtual void addCustomItem(std::shared_ptr<WorkItem> t) = 0;
    virtual void requireApproval(const std::string &trigger, ApprovalHandler *chain) = 0;
    virtual WorkflowDefinition *getResult() = 0;
};

#endif
