#ifndef DEPTHFIRSTITERATOR_H
#define DEPTHFIRSTITERATOR_H

#include <stack>
#include "WorkflowIterator.h"

class DepthFirstIterator : public WorkflowIterator {
	private:
		WorkflowNode* root_;
		std::stack<WorkflowNode*> stack_;

		void pushChildrenOf(WorkflowNode* node);
	public:
		explicit DepthFirstIterator(WorkflowNode* root);
		void first() override;
		void next() override;
		WorkflowNode* current() override;
		bool isDone() override;
};

#endif // DEPTHFIRSTITERATOR_H
