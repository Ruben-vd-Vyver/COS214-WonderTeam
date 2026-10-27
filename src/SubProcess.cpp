#include "SubProcess.h"

SubProcess::SubProcess(std::string id, std::string name, int sourceVersion)
	: CompositeNode(std::move(id), std::move(name)),
	sourceVersion_(sourceVersion) {}

std::string SubProcess::label() const {
	return "SubProcess";
}

int SubProcess::getSourceVersion() const {
	return sourceVersion_;
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
