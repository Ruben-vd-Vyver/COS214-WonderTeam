#ifndef TIVIDYENGINE_H
#define TIVIDYENGINE_H

#include <map>
#include <memory>
#include <string>
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


class TividyEngine {
private:
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

    Record &record(const std::string &instanceId);
    const Record &record(const std::string &instanceId) const;
    WorkItem &itemRef(WorkflowInstance &inst, const std::string &itemId) const;
    Participant &actorRef(const std::string &name) const;
    void requireAssignee(const WorkItem &item, const Participant &who, const std::string &op) const;
    void run(WorkflowInstance &inst, WorkItemCommand *cmd, const std::string &op) const;

public:
    TividyEngine();
    ~TividyEngine();
    TividyEngine(const TividyEngine &) = delete;
    TividyEngine &operator=(const TividyEngine &) = delete;

    
    void registerParticipant(Participant *p);

    std::string deploy(WorkflowDirector &d, const std::string &name = "", int version = 1);

   
    std::string startInstance(const std::string &definitionId,
                              const std::vector<WorkflowObserver *> &observers = {});

    void setAssignment(const std::string &instanceId, AssignmentStrategy *s);
    void subscribe(const std::string &instanceId, WorkflowObserver *o);
    void assignNext(const std::string &instanceId, const std::string &itemId);

    
    void startItem(const std::string &instanceId, const std::string &itemId, const std::string &actor);
    void completeItem(const std::string &instanceId, const std::string &itemId, const std::string &actor);
    void rejectItem(const std::string &instanceId, const std::string &itemId,
                    const std::string &actor, const std::string &reason);

    bool undoLast(const std::string &instanceId);


    std::string statusReport(const std::string &instanceId) const;
};

#endif
