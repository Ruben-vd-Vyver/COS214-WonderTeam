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
		std::stack<WorkflowNode*> stack_;

		/**
		 * @brief Performs the push children of operation.
		 * @param node Value or object supplied by the caller.
		 */
		void pushChildrenOf(WorkflowNode* node);
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
};

#endif // DEPTHFIRSTITERATOR_H
