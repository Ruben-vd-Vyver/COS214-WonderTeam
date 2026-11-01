#ifndef WORKFLOWEVENT_H
#define WORKFLOWEVENT_H

#include <ctime>
#include <string>

/**
 * @brief Represents the work item abstraction in the TIVIDY workflow system.
 */
class WorkItem;
/**
 * @brief Represents the workflow instance abstraction in the TIVIDY workflow system.
 */
class WorkflowInstance;

//Defines event data shared btn subject & observers
/**
 * @brief Represents the workflow event abstraction in the TIVIDY workflow system.
 */
struct WorkflowEvent {
    private:
        std::string type; //eg. DeadlineSet,ApprovalRequested/Approved/Rejected
        WorkItem* item;
        WorkflowInstance* instance;
        std::string detail; //general info
        std::time_t timestamp; //when created
        std::time_t deadline;
        bool hasDeadline;


    public:
        /**
         * @brief Constructs an object of this type.
         * @param type Value or object supplied by the caller.
         * @param item Value or object supplied by the caller.
         * @param instance Value or object supplied by the caller.
         * @param detail Value or object supplied by the caller.
         * @param deadline Value or object supplied by the caller.
         * @param hasDeadline Value or object supplied by the caller.
         */
        WorkflowEvent(const std::string& type,WorkItem* item, WorkflowInstance* instance,
                     const std::string& detail,std::time_t deadline = 0, bool hasDeadline = false);
        /**
         * @brief Returns the type.
         * @return Result produced by the operation.
         */
        std::string getType() const;
        /**
         * @brief Returns the item.
         * @return Result produced by the operation.
         */
        WorkItem* getItem() const;
        /**
         * @brief Returns the instance.
         * @return Result produced by the operation.
         */
        WorkflowInstance* getInstance() const;
        /**
         * @brief Returns the detail.
         * @return Result produced by the operation.
         */
        std::string getDetail() const;
        /**
         * @brief Returns the timestamp.
         * @return Result produced by the operation.
         */
        std::time_t getTimestamp() const;
        /**
         * @brief Returns the deadline.
         * @return Result produced by the operation.
         */
        std::time_t getDeadline() const;
        /**
         * @brief Reports whether the requested condition is satisfied.
         * @return Result produced by the operation.
         */
        bool hasDeadlineValue() const;

};

#endif
