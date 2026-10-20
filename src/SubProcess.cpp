#include "SubProcess.h"

SubProcess::SubProcess(std::string id, std::string name) : CompositeNode(
		std::move(id), std::move(name)) {}

std::string SubProcess::label() const {
	return "SubProcess";
}

bool SubProcess::isBlocked() const {
	return hasFailed(); 
}

void SubProcess::activate() {
	if(isBlocked()) {
		std::cout <<" ! SubProcess '" <<
			name_ << "' is blocked - waiting for a user\n";
		return;
	}
	CompositeNode::activate();
}
