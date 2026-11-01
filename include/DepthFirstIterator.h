#ifndef DEPTHFIRSTITERATOR_H
#define DEPTHFIRSTITERATOR_H

#include <stack>
#include "WorkflowIterator.h"

/**
 * @brief Represents the depth first iterator abstraction in the TIVIDY workflow system.
 */
class DepthFirstIterator : public WorkflowIterator {
	private:
		WorkflowNode* root_;
		struct Frame { WorkflowNode* node; int depth; Frame(WorkflowNode* n, int d) : node(n), depth(d) {} };
		std::stack<Frame> stack_;

		/**
		 * @brief Performs the push children of operation.
		 * @param node Value or object supplied by the caller.
		 * @param parentDepth Depth of the node whose children are pushed.
		 */
		void pushChildrenOf(WorkflowNode* node, int parentDepth);
	public:
		/**
		 * @brief Constructs an object of this type.
		 * @param root Value or object supplied by the caller.
		 */
		explicit DepthFirstIterator(WorkflowNode* root);
		/**
		 * @brief Performs the first operation.
		 */
		void first() override;
		/**
		 * @brief Performs the next operation.
		 */
		void next() override;
		/**
		 * @brief Performs the current operation.
		 * @return Result produced by the operation.
		 */
		WorkflowNode* current() override;
		/**
		 * @brief Reports whether the requested condition is satisfied.
		 * @return Result produced by the operation.
		 */
		bool isDone() override;
		/**
		 * @brief Returns the current node's depth relative to the iterator root.
		 * @return The current node's depth, or -1 if the iterator is done.
		 */
		int currentDepth() const override;
};

#endif // DEPTHFIRSTITERATOR_H
