#ifndef DEPTHFIRSTITERATOR_H
#define DEPTHFIRSTITERATOR_H

#include <stack>
#include "WorkIterator.h"

// Iterator pattern: ConcreteIterator for composite nodes.
// Visits every node below root in depth-first (pre-order) order, i.e. the same top-to-bottom order the tree is printed in:
//   Stage            (root, not visited itself)
//     Task A         1
//     SubProcess     2
//       Task B       3
//     Task C         4
//
// The iterator does not own any nodes. The tree must outlive the iterator, and the tree should not be changed (add/remove) while iterating.
class DepthFirstIterator : public WorkIterator {
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
