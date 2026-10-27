#include "WorkflowNode.h"

namespace {
	int counter = 0;
}

WorkflowNode::WorkflowNode(std::string name) : 
	id_("n" + std::to_string(++counter)), name_(std::move(name)) {}

WorkflowNode::WorkflowNode(std::string id, std::string name) :
	id_(std::move(id)), name_(std::move(name)) {}

const std::string& WorkflowNode::getId() const {
	return id_;
}

std::string WorkflowNode::getName() const {
	return name_;
}
