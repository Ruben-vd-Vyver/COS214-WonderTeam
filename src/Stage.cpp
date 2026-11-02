#include "Stage.h"

Stage::Stage(std::string id, std::string name, bool parallel) : 
	CompositeNode(std::move(id), std::move(name)),
	parallel_(parallel) {}

WorkflowNode* Stage::clone() const {
	Stage* copy = new Stage(id_, name_, parallel_);
	cloneChildrenInto(*copy);
	return copy;
}

bool Stage::isParallel() const {
	return parallel_;
}

void Stage::activate() {
	for (auto* c:children_) {
		if(c->isComplete()) continue;
		c->activate();
		if(!parallel_) return;
	}
}
