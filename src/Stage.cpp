#include "Stage.h"

Stage::Stage(std::string id, std::string name, bool parallel)
	: CompositeNode(std::move(id), std::move(name)), parallel_(parallel) {}

std::string Stage::label() const { return parallel_ ?
	"Stage|parallel" : "Stage|sequential";}

bool Stage::isParallel() const {
	return parallel_;
}

void Stage::activate() {
	if (parallel_) {
		CompositeNode::activate();
		return;
	}
	for (auto& c : children_)
		if(!c->isComplete()) {
			c->activate();
			return;
		}
}
