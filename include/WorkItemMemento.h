#ifndef WORKITEMMEMENTO_H
#define WORKITEMMEMENTO_H

#include <map>
#include <memory>
#include <string>

/**
 * @brief Represents the work item state abstraction in the TIVIDY workflow system.
 */
class WorkItemState;
/**
 * @brief Represents the participant abstraction in the TIVIDY workflow system.
 */
class Participant;
/**
 * @brief Represents the basic work item abstraction in the TIVIDY workflow system.
 */
class BasicWorkItem;

/**
 * @brief Represents the work item memento abstraction in the TIVIDY workflow system.
 */
class WorkItemMemento {
    friend class BasicWorkItem;

private:
    std::unique_ptr<WorkItemState> savedState_;
    Participant* assignee_;
    std::map<std::string, std::string> payload_;

    /**
     * @brief Constructs an object of this type.
     * @param state Value or object supplied by the caller.
     * @param assignee Value or object supplied by the caller.
     * @param data Value or object supplied by the caller.
     */
    WorkItemMemento(WorkItemState* state, Participant* assignee,
                    std::map<std::string, std::string> data);

public:
    /**
     * @brief Destroys this object and releases owned resources.
     */
    ~WorkItemMemento();
    /**
     * @brief Constructs an object of this type.
     */
    WorkItemMemento(const WorkItemMemento&) = delete;
    WorkItemMemento& operator=(const WorkItemMemento&) = delete;
    /**
     * @brief Constructs an object of this type.
     */
    WorkItemMemento(WorkItemMemento&&) = default;
    WorkItemMemento& operator=(WorkItemMemento&&) = default;
};

#endif
