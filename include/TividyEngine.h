#ifndef TIVIDYENGINE_H
#define TIVIDYENGINE_H

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class Participant;
class WorkflowDirector;
class WorkflowDefinition;
class WorkflowInstance;
class WorkflowObserver;
class WorkflowNode;
class WorkflowSubject;
class AssignmentStrategy;
class WorkItem;
class WorkItemCommand;

// Optional wrappers applied when a WorkItem is created through the Facade.
struct WorkItemDecoratorOptions {
    bool validateRequiredFields;
    std::vector<std::string> permittedRoles; // empty means no SecuredItem wrapper
    bool usePriority;
    int priority;
    WorkItemDecoratorOptions()
        : validateRequiredFields(false), usePriority(false), priority(3) {}
};


class TividyEngine {
private:
    struct Record {
        std::unique_ptr<WorkflowInstance> instance;
        std::string definitionId;
        bool hasStrategy = false;
        std::unique_ptr<AssignmentStrategy> ownedStrategy;
        std::string strategyName;
    };

    std::vector<Participant *> participants_;                                
    std::map<std::string, std::unique_ptr<WorkflowDefinition>> definitions_; 
    std::map<std::string, Record> records_;                                  
    int definitionCount_;
    int instanceCount_;

    Record &record(const std::string &instanceId);
    const Record &record(const std::string &instanceId) const;
    WorkItem &itemRef(WorkflowInstance &inst, const std::string &itemId) const;
    Participant &actorRef(const std::string &name) const;
    void requireAssignee(const WorkItem &item, const Participant &who, const std::string &op) const;
    void run(WorkflowInstance &inst, WorkItemCommand *cmd, const std::string &op) const;

public:
    // Read-only projection of the Composite workflow tree for presentation layers.
    // It contains values only; callers never receive mutable WorkItem pointers.
    struct WorkflowEntry {
        int depth;
        bool isWorkItem;
        std::string id;
        std::string name;
        std::string state;
        std::string assignee;
        std::string requiredRole;
        // Populated for Pending work items when an unfinished sibling in an
        // ancestor sequential Stage prevents this item from being activated.
        std::string availabilityBlockerId;
        std::string availabilityBlockerName;
        std::string availabilityBlockerState;
        std::string availabilityStageName;
        std::vector<std::pair<std::string, std::string> > requiredFields;
        std::vector<std::string> decorators;
    };

    TividyEngine();
    ~TividyEngine();
    TividyEngine(const TividyEngine &) = delete;
    TividyEngine &operator=(const TividyEngine &) = delete;

    
    void registerParticipant(Participant *p);

    std::string deploy(WorkflowDirector &d, const std::string &name = "", int version = 1);

   
    std::string startInstance(const std::string &definitionId,
                              const std::vector<WorkflowObserver *> &observers = {});

    void setAssignment(const std::string &instanceId, AssignmentStrategy *s);
    // Facade-owned named strategies can be switched safely while the instance is live.
    void setAssignmentMode(const std::string &instanceId, const std::string &mode);
    std::string assignmentMode(const std::string &instanceId) const;
    void subscribe(const std::string &instanceId, WorkflowObserver *o);
    void assignNext(const std::string &instanceId, const std::string &itemId);
    // Explicit assignment still executes AssignCommand inside the Facade.
    void assignItem(const std::string &instanceId, const std::string &itemId,
                    const std::string &participantName);

    
    void startItem(const std::string &instanceId, const std::string &itemId, const std::string &actor);
    void completeItem(const std::string &instanceId, const std::string &itemId, const std::string &actor);
    void rejectItem(const std::string &instanceId, const std::string &itemId,
                    const std::string &actor, const std::string &reason);

    bool undoLast(const std::string &instanceId);

    // Run the task's configured Chain of Responsibility after a completion
    // request has moved the item into AwaitingApproval. The result is applied
    // via ApproveCommand / RejectCommand and recorded in CommandHistory.
    std::string submitApproval(const std::string &instanceId, const std::string &itemId,
                               const std::string &requesterName, int requiredLevel,
                               const std::string &trigger = "complete");

    // Structured read model for a TUI; preserves the Facade boundary.
    std::vector<WorkflowEntry> workflowTree(const std::string &instanceId) const;
    // Depth-first projection produced by the Iterator pattern; used by Overview.
    std::vector<WorkflowEntry> workflowOverview(const std::string &instanceId) const;

    // Runtime workflow authoring. New nodes are attached to the live Composite
    // tree through the Facade and automatically inherit the instance Observer.
    // createTask() adds a top-level task to the workflow root; createSubtask()
    // adds a task beneath an existing Stage/SubProcess.
    std::string createTask(const std::string &instanceId, const std::string &name,
                           const std::string &requiredRole,
                           const std::vector<std::string> &requiredFields = std::vector<std::string>(),
                           const WorkItemDecoratorOptions &decorators = WorkItemDecoratorOptions());
    std::string createSubtask(const std::string &instanceId, const std::string &parentCompositeId,
                              const std::string &name, const std::string &requiredRole,
                              const std::vector<std::string> &requiredFields = std::vector<std::string>(),
                              const WorkItemDecoratorOptions &decorators = WorkItemDecoratorOptions());
    // type must be "stage" or "subprocess". Stage nodes support sequential/parallel execution.
    std::string createComposite(const std::string &instanceId, const std::string &parentCompositeId,
                                const std::string &name, const std::string &type,
                                bool parallel = false, int sourceVersion = 1);

    // Update payload data through the Facade (state transitions remain commands).
    void setItemField(const std::string &instanceId, const std::string &itemId,
                      const std::string &actor, const std::string &field,
                      const std::string &value);

    std::string statusReport(const std::string &instanceId) const;
};

#endif
