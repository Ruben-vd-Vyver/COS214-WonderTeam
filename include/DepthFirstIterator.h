#ifndef DEPTHFIRSTITERATOR_H
#define DEPTHFIRSTITERATOR_H

#include <stack>
#include "WorkflowIterator.h"

class DepthFirstIterator : public WorkflowIterator {
	private:
		WorkflowNode* root_;
		struct Frame { WorkflowNode* node; int depth; Frame(WorkflowNode* n, int d) : node(n), depth(d) {} };
		std::stack<Frame> stack_;

		void pushChildrenOf(WorkflowNode* node, int parentDepth);
	public:
		explicit DepthFirstIterator(WorkflowNode* root);
		void first() override;
		void next() override;
		WorkflowNode* current() override;
		bool isDone() override;
		int currentDepth() const override;
};

#endif // DEPTHFIRSTITERATOR_H
