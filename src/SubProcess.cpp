#include "SubProcess.h"

SubProcess::SubProcess(std::string name, int sourceVersion) :
	CompositeNode(std::move(name)), sourceVersion_(sourceVersion) {}

int SubProcess::getSourceVersion() const {
	return sourceVersion_;
}

WorkflowNode* SubProcess::clone() const {
	SubProcess* copy = new SubProcess(name_, sourceVersion_);
	cloneChildrenInto(*copy);
	return copy;
}

void SubProcess::activate() {
	for (auto* c : children_)
		if(!c->isComplete()) c->activate():
}
