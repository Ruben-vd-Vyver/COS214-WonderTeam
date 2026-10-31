#ifndef WORKFLOWINSTANCE_H
#define WORKFLOWINSTANCE_H

#include <ctime>
#include <map>
#include <memory>
#include <string>

#include "WorkflowSubject.h"
#include "CommandHistory.h"
#include "WorkAllocator.h"

class WorkItem;
class WorkItemCommand;
class WorkflowNode;
class WorkflowDefinition;

// ConcreteSubject
class WorkflowInstance : public WorkflowSubject
{
private:
    std::string instanceId;
    std::map<std::string, std::string> data;
    std::unique_ptr<WorkflowNode> root_;
    CommandHistory history_;
    WorkAllocator allocator_;
    void publishEvent(const std::string &type, WorkItem *item, const std::string &detail, std::time_t deadline = 0, bool hasDeadline = false);

public:
    WorkflowInstance(const WorkflowDefinition &def, WorkflowNode *root, const std::string &id);
    explicit WorkflowInstance(const std::string &instanceId);
    ~WorkflowInstance();
    WorkflowInstance(const WorkflowInstance &) = delete;
    WorkflowInstance &operator=(const WorkflowInstance &) = delete;
    const std::string &getInstanceId() const;
    void start();

    void perform(WorkItemCommand *cmd);
    bool undoLast();
    WorkflowNode *getRoot() const;
    WorkItem *findItem(const std::string &id) const;
    bool isComplete() const;
    WorkAllocator &getAllocator();
    void setData(const std::string &key, const std::string &value);
    bool getData(const std::string &key, std::string &value) const;
    std::string getData(const std::string &key) const;

    void createWorkItem(WorkItem *item);
    void requestApproval(WorkItem *item);
    void approve(WorkItem *item);
    void reject(WorkItem *item);
    void completeWorkItem(WorkItem *item);
    void setDeadline(WorkItem *item, std::time_t deadline);
};

#endif // WORKFLOWINSTANCE_H
