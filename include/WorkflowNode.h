#ifndef WORKFLOWNODE_H
#define WORKFLOWNODE_H

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
class WorkItem;
class WorkflowIterator;
class WorkflowSubject;

class WorkflowNode {
	protected:
		std::string id_;
		std::string name_;
		WorkflowNode(std::string id, std::string name);
	public:
		virtual ~WorkflowNode() = default;
		WorkflowNode(const WorkflowNode&) =delete;
		WorkflowNode& operator=(const WorkflowNode&) = delete;

		const std::string& getId() const;
		std::string getName() const;

		virtual WorkflowNode* clone() const=0;
		virtual void activate() = 0;
		virtual bool isComplete() const = 0;
		virtual void bind(WorkflowSubject* s) = 0;
		virtual WorkItem* find(const std::string& id) = 0;
		virtual WorkflowIterator* createIterator() = 0;

		virtual int childCount() const;
		virtual WorkflowNode* child(int i) const;
		virtual WorkItem* asWorkItem();
};

#endif
