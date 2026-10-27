#include <string>
#include <WorkflowNode.h>
#include <WorkItemMemento.h>

class Participant;

class WorkItem : public WorkflowNode {
	protected:
		explicit WorkItem(std::string name);
		WorkItem(std::string id, std::string name);
	public:
		virtual void assign(Participant* p) = 0;
		virtual void start() = 0;
		virtual void complete() =0;
		virtual void reject(const std::string& reason) =0;
		virtual void escalate() = 0;
		virtual std::string getRequiredRole() const =0;
		virtual std::string getStateName() const=0;
		virtual WorkItemMemento createMemento() const=0;
		virtual void restore(const WorkItemMemento& m)=0;

		//Leaf behaviour shared by all work items
		WorkItem* find(const std::string& id) override;
		WorkflowIterator* createIterator() override; //NUllIterator
