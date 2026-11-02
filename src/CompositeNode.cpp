#include "CompositeNode.h"
#include <algorithm>
#include <stdexcept>
#include "DepthFirstIterator.h"

namespace {
	bool subtreeContains(const WorkflowNode* root, 
			const WorkflowNode* target) {
		if (root == target) return true;
		for (int i=0; i<root->childCount(); ++i) {
			if (subtreeContains(root->child(i), target)) {
				return true;
			}
		}
		return false;
	}
}

CompositeNode::CompositeNode(std::string id, std::string name) :
	WorkflowNode (std::move(id), std::move(name)) 
{}

CompositeNode :: ~CompositeNode() {
	for (auto* c : children_) delete c;
}

void CompositeNode::add(WorkflowNode* node) {
	if(!node) throw std::invalid_argument("cannot add a null node");
	if (subtreeContains(this, node)) throw
		std::invalid_argument("node is already in this workflow");
	if (subtreeContains(node, this)) throw 
		std::invalid_argument ("adding this node creates a cycle");
	children_.push_back(node);
	if (subject_) node->bind(subject_);
}

void CompositeNode::remove(WorkflowNode* node) {
	auto it = std::find(children_.begin(), children_.end(), node);
	if (it == children_.end()) throw std::invalid_argument( 
			"node is not a direct child");
	children_.erase(it);
	delete node;
}

bool CompositeNode::isComplete() const {
	if (children_.empty()) return false;
	return std::all_of(children_.begin(), children_.end(), [] 
			(const WorkflowNode* c) { return c-> isComplete();});
}

int CompositeNode::childCount() const {
	return static_cast<int>(children_.size());
}

WorkflowNode* CompositeNode::child(int i) const {
	if (i<0 || i>= childCount()) throw 
		std::out_of_range("child index is out of range");
	return children_[i];
}

void CompositeNode::bind(WorkflowSubject* s) {
	subject_ = s;
	for (auto* c : children_) c->bind(s);
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
