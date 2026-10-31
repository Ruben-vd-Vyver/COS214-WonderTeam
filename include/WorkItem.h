#ifndef WORKITEM_H
#define WORKITEM_H

#include <memory>
#include <string>
#include "WorkflowNode.h"
#include "WorkItemMemento.h"
#include "Task.h"

class Participant;

class WorkItem : public WorkflowNode {
	protected:
		WorkItem(std::string id, std::string name);
	public:
		virtual void assign(Participant* p) = 0;
		virtual void start() = 0;
		virtual void complete() =0;
		virtual void reject(const std::string& reason) =0;
		virtual void escalate() = 0;
		virtual void approve() = 0;
		virtual bool isActive() const = 0;
		virtual const Task& getTask() const = 0;
		virtual std::string getRequiredRole() const =0;
		virtual std::string getStateName() const=0;
		virtual Participant* getAssignee() const =0;
		virtual void setField(const std::string& key,
				const std::string& value) =0;
		virtual std::string getField(const std::string& key) const=0;
		virtual std::unique_ptr<WorkItemMemento> createMemento() const=0;
		virtual void restore(const WorkItemMemento& m)=0;
		WorkItem* clone() const override =0;

		//Leaf behaviour shared by all work items
		WorkItem* find(const std::string& id) override;
		WorkflowIterator* createIterator() override; //NUllIterator
		WorkItem* asWorkItem() override;
};

#endif
