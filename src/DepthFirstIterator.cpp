#include "DepthFirstIterator.h"
#include "WorkflowNode.h"

DepthFirstIterator::DepthFirstIterator(WorkflowNode* root) : root_(root) {
	first();
}

void DepthFirstIterator::pushChildrenOf(WorkflowNode* node) {
	if (!node) return;
	for (int i = node->childCount(); i > 0; --i)
		stack_.push(node->child(i - 1));
}

void DepthFirstIterator::first() {
	while (!stack_.empty()) stack_.pop();
	pushChildrenOf(root_);
}

void DepthFirstIterator::next() {
	if (stack_.empty()) return;
	WorkflowNode* visited = stack_.top();
	stack_.pop();
	pushChildrenOf(visited); 
}

WorkflowNode* DepthFirstIterator::current() {
	return stack_.empty() ? nullptr : stack_.top();
}

bool DepthFirstIterator::isDone() {
	return stack_.empty();
}
