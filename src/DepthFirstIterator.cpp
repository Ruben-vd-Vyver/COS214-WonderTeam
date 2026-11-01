#include "DepthFirstIterator.h"
#include "WorkflowNode.h"

DepthFirstIterator::DepthFirstIterator(WorkflowNode* root) : root_(root) {
    first();
}

void DepthFirstIterator::pushChildrenOf(WorkflowNode* node, int parentDepth) {
    if (!node) return;
    for (int i = node->childCount(); i > 0; --i)
        stack_.push(Frame(node->child(i - 1), parentDepth + 1));
}

void DepthFirstIterator::first() {
    while (!stack_.empty()) stack_.pop();
    pushChildrenOf(root_, 0);
}

void DepthFirstIterator::next() {
    if (stack_.empty()) return;
    const Frame visited = stack_.top();
    stack_.pop();
    pushChildrenOf(visited.node, visited.depth);
}

WorkflowNode* DepthFirstIterator::current() {
    return stack_.empty() ? nullptr : stack_.top().node;
}

bool DepthFirstIterator::isDone() {
    return stack_.empty();
}

int DepthFirstIterator::currentDepth() const {
    return stack_.empty() ? -1 : stack_.top().depth;
}
