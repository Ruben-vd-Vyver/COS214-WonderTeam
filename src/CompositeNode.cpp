#include "CompositeNode.h"
#include <algorithm>
#include <stdexcept>
#include "DepthFirstIterator.h"
#include "WorkItem.h"

CompositeNode::CompositeNode(std::string name) : WorkflowNode (std::move(name)) 
{}

CompositeNode :: ~CompositeNode() {
	for (auto* c : children_) delete c;
}

bool CompositeNode::contains(const WorkflowNode* node) const {
	for (const auto* c : children_) {
		if (c==node) return true;
		if (const auto* comp = dynamic_cast<const CompositeNode*>(c))
			if(comp->contains(node)) return true;
	}
	return false;
}

void CompositeNode::add(WorkflowNode* node) {
	if(!node) throw std::invalid_argument("cannot add a null node");
	if (node==this || contains(node)) throw
		std::invalid_argument("node is already in this workflow");
	if (const auto* comp = dynamic_cast<const CompositeNode*>(node))
		if (comp->contains(this)) throw 
			std::invalid_argument ("adding this node creates a cycle");
	children_.push_back(node);
}

void CompositeNode::remove(WorkflowNode* node {
	auto it = std::find(children_.begin(), children_.end(), node);
	if (it == children_.end()) throw std::invalid_argument( 
			"node is not a direct child");
	children_.erase(it);
	delete node;
}
const std::vector<std::shared_ptr<WorkflowNode>>& CompositeNode::getChildren()
	const {return children_;}

bool CompositeNode::isComplete() const {
	if (children_.empty()) return false;
	return std::all_of(children_.begin(), children_.end(), [] 
			(const WorkflowNode* c) { return c-> isComplete();});
}

WorkItem* CompositeNode::find(const std::string& id) {
	for (auto* c : children_)
		if (WorkItem* hit = c->find(id)) return hit;
	return nullptr;
}

WorkflowIterator* CompositeNode::createIterator() {
	return new DepthFirstIterator(this);
}

void CompositeNode::cloneChildrenInto(CompositeNode& target) const {
	for (const auto* c : children_) target.add(c->clone());
}
