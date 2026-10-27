#ifndef WORKFLOWNODE_H
#define WORKFLOWNODE_H

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

// Composite: Component
// Prototype: Prototype (clone() is the prototype operation)
class WorkflowNode {
	protected:
		std::string id_;
		std::string name_;

		// [Prototype] copy constructor is protected so that only clone() can copy a node. 
		// Outside code can never accidentally slice a node by copying it through a base reference.
		WorkflowNode(const WorkflowNode& other) = default;
	public:
		WorkflowNode(std::string id, std::string name);
		virtual ~WorkflowNode() = default;
		WorkflowNode& operator=(const WorkflowNode&) = delete;

		const std::string& getId() const;
		const std::string& getName() const;

		//The following handle all the children
		virtual void add(std::shared_ptr<WorkflowNode> node);
		virtual bool remove(const std::string& id);
		virtual std::shared_ptr<WorkflowNode> getChild
			(const std::string& id) const;
		virtual const std::vector<std::shared_ptr<WorkflowNode>>& getChildren() const;
		//end of child management, now utilities

		virtual void activate() =0;
		virtual bool isComplete() const=0;
		virtual bool hasFailed() const;
		virtual void print(int indent=0) const=0;

		// [Prototype] Returns a deep copy of this node (and its whole subtree). 
		// The caller owns the returned pointer, e.g. wrap it: std::shared_ptr<WorkflowNode> copy(node->clone());
		// The copy keeps the same ids, so findItem(id) works the same in every instance created from one definition. 
		virtual WorkflowNode* clone() const = 0;
};

#endif // WORKFLOWNODE_H
