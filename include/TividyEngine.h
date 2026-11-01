#ifndef TIVIDYENGINE_H
#define TIVIDYENGINE_H

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

/**
 * @brief Represents the participant abstraction in the TIVIDY workflow system.
 */
class Participant;
/**
 * @brief Represents the workflow director abstraction in the TIVIDY workflow system.
 */
class WorkflowDirector;
/**
 * @brief Represents the workflow definition abstraction in the TIVIDY workflow system.
 */
class WorkflowDefinition;
/**
 * @brief Represents the workflow instance abstraction in the TIVIDY workflow system.
 */
class WorkflowInstance;
/**
 * @brief Represents the workflow observer abstraction in the TIVIDY workflow system.
 */
class WorkflowObserver;
/**
 * @brief Represents the workflow node abstraction in the TIVIDY workflow system.
 */
class WorkflowNode;
/**
 * @brief Represents the workflow subject abstraction in the TIVIDY workflow system.
 */
class WorkflowSubject;
/**
 * @brief Represents the assignment strategy abstraction in the TIVIDY workflow system.
 */
class AssignmentStrategy;
/**
 * @brief Represents the work item abstraction in the TIVIDY workflow system.
 */
class WorkItem;
/**
 * @brief Represents the work item command abstraction in the TIVIDY workflow system.
 */
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


/**
 * @brief Represents the tividy engine abstraction in the TIVIDY workflow system.
 */
class TividyEngine {
private:
    /**
     * @brief Represents the record abstraction in the TIVIDY workflow system.
     */
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

    /**
     * @brief Performs the record operation.
     * @param instanceId Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    Record &record(const std::string &instanceId);
    /**
     * @brief Performs the record operation.
     * @param instanceId Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    const Record &record(const std::string &instanceId) const;
    /**
     * @brief Performs the item ref operation.
     * @param inst Value or object supplied by the caller.
     * @param itemId Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    WorkItem &itemRef(WorkflowInstance &inst, const std::string &itemId) const;
    /**
     * @brief Performs the actor ref operation.
     * @param name Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    Participant &actorRef(const std::string &name) const;
    /**
     * @brief Performs the require assignee operation.
     * @param item Value or object supplied by the caller.
     * @param who Value or object supplied by the caller.
     * @param op Value or object supplied by the caller.
     */
    void requireAssignee(const WorkItem &item, const Participant &who, const std::string &op) const;
    /**
     * @brief Performs the run operation.
     * @param inst Value or object supplied by the caller.
     * @param cmd Value or object supplied by the caller.
     * @param op Value or object supplied by the caller.
     */
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

    /**
     * @brief Constructs an object of this type.
     */
    TividyEngine();
    /**
     * @brief Destroys this object and releases owned resources.
     */
    ~TividyEngine();
    /**
     * @brief Constructs an object of this type.
     */
    TividyEngine(const TividyEngine &) = delete;
    TividyEngine &operator=(const TividyEngine &) = delete;

    
    /**
     * @brief Registers the supplied object with this component.
     * @param p Value or object supplied by the caller.
     */
    void registerParticipant(Participant *p);

    /**
     * @brief Performs the deploy operation.
     * @param d Value or object supplied by the caller.
     * @param name Value or object supplied by the caller.
     * @param version Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    std::string deploy(WorkflowDirector &d, const std::string &name = "", int version = 1);

   
    /**
     * @brief Starts the requested workflow operation.
     * @param definitionId Value or object supplied by the caller.
     * @param observers Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    std::string startInstance(const std::string &definitionId,
                              const std::vector<WorkflowObserver *> &observers = {});

    /**
     * @brief Sets the assignment.
     * @param instanceId Value or object supplied by the caller.
     * @param s Value or object supplied by the caller.
     */
    void setAssignment(const std::string &instanceId, AssignmentStrategy *s);
    // Facade-owned named strategies can be switched safely while the instance is live.
    /**
     * @brief Selects a built-in assignment strategy for a workflow instance.
     * @param instanceId ID of the workflow instance.
     * @param mode Strategy name: role, workload, or rule (including their
     *             role-based, least-workload, and rule-based aliases).
     * @throws std::invalid_argument if the mode is not recognized.
     */
    void setAssignmentMode(const std::string &instanceId, const std::string &mode);
    /**
     * @brief Returns the configured assignment strategy name.
     * @param instanceId ID of the workflow instance.
     * @return The strategy name, or "Not configured" if none is set.
     */
    std::string assignmentMode(const std::string &instanceId) const;
    /**
     * @brief Performs the subscribe operation.
     * @param instanceId Value or object supplied by the caller.
     * @param o Value or object supplied by the caller.
     */
    void subscribe(const std::string &instanceId, WorkflowObserver *o);
    /**
     * @brief Performs the assign next operation.
     * @param instanceId Value or object supplied by the caller.
     * @param itemId Value or object supplied by the caller.
     */
    void assignNext(const std::string &instanceId, const std::string &itemId);
    // Explicit assignment still executes AssignCommand inside the Facade.
    void assignItem(const std::string &instanceId, const std::string &itemId,
                    const std::string &participantName);

    
    /**
     * @brief Starts the requested workflow operation.
     * @param instanceId Value or object supplied by the caller.
     * @param itemId Value or object supplied by the caller.
     * @param actor Value or object supplied by the caller.
     */
    void startItem(const std::string &instanceId, const std::string &itemId, const std::string &actor);
    /**
     * @brief Completes the requested workflow operation.
     * @param instanceId Value or object supplied by the caller.
     * @param itemId Value or object supplied by the caller.
     * @param actor Value or object supplied by the caller.
     */
    void completeItem(const std::string &instanceId, const std::string &itemId, const std::string &actor);
    /**
     * @brief Rejects the operation and records the rejection reason.
     * @param instanceId Value or object supplied by the caller.
     * @param itemId Value or object supplied by the caller.
     * @param actor Value or object supplied by the caller.
     * @param reason Value or object supplied by the caller.
     */
    void rejectItem(const std::string &instanceId, const std::string &itemId,
                    const std::string &actor, const std::string &reason);

    /**
     * @brief Performs the undo last operation.
     * @param instanceId Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    bool undoLast(const std::string &instanceId);

    // Run the task's configured Chain of Responsibility after a completion
    // request has moved the item into AwaitingApproval. The result is applied
    // via ApproveCommand / RejectCommand and recorded in CommandHistory.
    std::string submitApproval(const std::string &instanceId, const std::string &itemId,
                               const std::string &requesterName, int requiredLevel,
                               const std::string &trigger = "complete");

    // Structured read model for a TUI; preserves the Facade boundary.
    /**
     * @brief Returns workflow entries in tree traversal order.
     * @param instanceId ID of the workflow instance.
     * @return Workflow entries annotated with availability reasons.
     */
    std::vector<WorkflowEntry> workflowTree(const std::string &instanceId) const;
    // Depth-first projection produced by the Iterator pattern; used by Overview.
    /**
     * @brief Returns a depth-first workflow overview.
     * @param instanceId ID of the workflow instance.
     * @return Workflow entries including their depth in the tree and
     *         availability reasons.
     */
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

    /**
     * @brief Performs the status report operation.
     * @param instanceId Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    std::string statusReport(const std::string &instanceId) const;
};

#endif
