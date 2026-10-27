#include "DepthFirstIterator.h"
#include "WorkflowNode.h"

DepthFirstIterator::DepthFirstIterator(WorkflowNode* root) : root_(root) {
	first();
}

// Children are pushed in reverse so the first child ends up on top of the stack and is visited first.
void DepthFirstIterator::pushChildrenOf(WorkflowNode* node) {
	if (!node) return;
	const std::vector<std::shared_ptr<WorkflowNode> >& kids = node->getChildren();
	for (std::size_t i = kids.size(); i > 0; --i)
		stack_.push(kids[i - 1].get());
}

void DepthFirstIterator::first() {
	while (!stack_.empty()) stack_.pop();
	pushChildrenOf(root_);
}

void DepthFirstIterator::next() {
	if (stack_.empty()) return;
	WorkflowNode* visited = stack_.top();
	stack_.pop();
	pushChildrenOf(visited); // go deeper before moving on to siblings
}

WorkflowNode* DepthFirstIterator::current() {
	return stack_.empty() ? nullptr : stack_.top();
}

bool DepthFirstIterator::isDone() {
	return stack_.empty();
}
