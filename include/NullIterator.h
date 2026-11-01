#ifndef NULLITERATOR_H
#define NULLITERATOR_H

#include "WorkflowIterator.h"

/**
 * @brief Represents the null iterator abstraction in the TIVIDY workflow system.
 */
class NullIterator : public WorkflowIterator {
	public:
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

#endif // NULLITERATOR_H
