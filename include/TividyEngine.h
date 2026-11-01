#ifndef TIVIDYENGINE_H
#define TIVIDYENGINE_H

#include <map>
#include <memory>
#include <string>
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


    /**
     * @brief Performs the status report operation.
     * @param instanceId Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    std::string statusReport(const std::string &instanceId) const;
};

#endif
