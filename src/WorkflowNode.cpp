#include "WorkflowNode.h"

WorkflowNode::WorkflowNode(std::string id, std::string name)
	: id_(std::move(id)), name_(std::move(name)) {}

const std::string& WorkflowNode::getId() const {return id_;}

const std::string& WorkflowNode::getName() const {return name_;}

void WorkflowNode::add(std::shared_ptr<WorkflowNode>) {
	throw std::logic_error(
			"'"+name_+"' is a leaf and cannot have children");
}

bool WorkflowNode::remove(const std::string&) {
	throw std::logic_error(
			"'"+name_+"' is a leaf and has no children to remove");
}

std::shared_ptr<WorkflowNode> WorkflowNode::getChild(const std::string&) 
	const {return nullptr;}

const std::vector<std::shared_ptr<WorkflowNode>>& WorkflowNode::getChildren() 
	const {
		static const std::vector<std::shared_ptr<WorkflowNode>> none;
		return none;
	}

bool WorkflowNode::hasFailed() const { return false;}
