#include "CompositeNode.h"
#include <algorithm>

CompositeNode::CompositeNode(std::string id, std::string name)
	: WorkflowNode(std::move(id), std::move(name)) {}

std::string CompositeNode::label() const { return "Composite"; }

void CompositeNode::add(std::shared_ptr<WorkflowNode> node) {
	if(!node) throw std::invalid_argument("cannot add a null node");
	if (node.get() == this) throw std::invalid_argument(
			"a node cannot contain itself");
	if (getChild(node->getId())) throw std::invalid_argument(
			"duplicate node id: " + node->getId());
	children_.push_back(std::move(node));
}

bool CompositeNode::remove(const std::string& id) {
	auto it = std::find_if(children_.begin(), children_.end(),
			[&](const auto& c) {
			return c->getId() == id;});
	if (it ==children_.end()) return false;
	children_.erase(it);
	return true;
}

std::shared_ptr<WorkflowNode> CompositeNode::getChild(const std::string& id)
	const {
		for (const auto& c:children_) if (c->getId() == id) return c;
		return nullptr;
	}

const std::vector<std::shared_ptr<WorkflowNode>>& CompositeNode::getChildren()
	const {return children_;}

void CompositeNode::activate() {
	for (auto& c : children_) if (!c->isComplete()) c-> activate();
}

bool CompositeNode::isComplete() const {
	if (children_.empty()) return false;
	return std::all_of(children_.begin(), children_.end(), [] 
			(const auto& c) { return c-> isComplete();});
}

bool CompositeNode::hasFailed() const {
	return std::any_of(children_.begin(), children_.end(), [](const auto& c)
			{return c->hasFailed();});
}

void CompositeNode::print(int indent) const {
	std::cout << std::string(indent, ' ') << "["<< label()<<"]"<<name_
		<< (isComplete() ? " (complete)" : hasFailed() ? " (Failed)"
				:"") << "\n";
	for (const auto& c : children_) c->print(indent+2);
}

