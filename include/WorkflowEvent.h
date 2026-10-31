#ifndef WORKFLOWEVENT_H
#define WORKFLOWEVENT_H

#include <ctime>
#include <string>

class WorkItem;
class WorkflowInstance;

//Defines event data shared btn subject & observers
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
        WorkflowEvent(const std::string& type,WorkItem* item, WorkflowInstance* instance,
                     const std::string& detail,std::time_t deadline = 0, bool hasDeadline = false);
        std::string getType() const;
        WorkItem* getItem() const;
        WorkflowInstance* getInstance() const;
        std::string getDetail() const;
        std::time_t getTimestamp() const;
        std::time_t getDeadline() const;
        bool hasDeadlineValue() const;

};

#endif