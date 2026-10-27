#ifndef WORKFLOWNODE_H
#define WORKFLOWNODE_H

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

class WorkIterator; // [Iterator]

// Composite: Component
// Iterator: Aggregate (createIterator() is the factory method)
class WorkflowNode {
	protected:
		std::string id_;
		std::string name_;
	public:
		WorkflowNode(std::string id, std::string name);
		virtual ~WorkflowNode() = default;
		const std::string& getId() const;
		const std::string& getName() const;

		//The following handle all the children
		virtual void add(std::shared_ptr<WorkflowNode> node);
		virtual bool remove(const std::string& id);
		virtual std::shared_ptr<WorkflowNode> getChild (const std::string& id) const;
		virtual const std::vector<std::shared_ptr<WorkflowNode>>& getChildren() const;
		//end of child management, now utilities

		virtual void activate() =0;
		virtual bool isComplete() const=0;
		virtual bool hasFailed() const;
		virtual void print(int indent=0) const=0;

		// [Iterator] Returns an iterator over every node below this one.
		// Leaves (Tasks) get a NullIterator by default; CompositeNode overrides this to return a DepthFirstIterator.
		// The caller owns the returned iterator and must delete it.
		virtual WorkIterator* createIterator();
};

#endif // WORKFLOWNODE_H
