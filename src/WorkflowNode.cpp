#include "WorkflowNode.h"

namespace {
	int counter = 0;
}

WorkflowNode::WorkflowNode(std::string id, std::string name) : 
	id_(std::move(id)), name_(std::move(name)) {}


const std::string& WorkflowNode::getId() const {
	return id_;
}

std::string WorkflowNode::getName() const {
	return name_;
}

int WorkflowNode::childCount() const {
	return 0;
}

WorkflowNode* WorkflowNode::child(int) const {
	throw std::out_of_range("node has no such child");
}

WorkItem* WorkflowNode::asWorkItem() {
	return nullptr;
}
