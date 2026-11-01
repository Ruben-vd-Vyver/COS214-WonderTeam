#ifndef WORKITERATOR_H
#define WORKITERATOR_H

/**
 * @brief Represents the workflow node abstraction in the TIVIDY workflow system.
 */
class WorkflowNode;

// Iterator pattern: abstract Iterator
// Walks the nodes *below* a WorkflowNode without the caller needing to know whether it is a Stage, SubProcess or a single Task.
// Usage:
/**
 * @brief Creates the iterator.
 * @return Result produced by the operation.
 */
//   WorkIterator* it = node->createIterator();
/**
 * @brief Performs the for operation.
 * @return Result produced by the operation.
 */
//   for (it->first(); !it->isDone(); it->next()) {
/**
 * @brief Performs the current operation.
 * @return Result produced by the operation.
 */
//       WorkflowNode* n = it->current();
//       ...
//   }
/**
 * @brief Performs the iterator operation.
 * @param nodes Value or object supplied by the caller.
 * @return Result produced by the operation.
 */
//   delete it;   // the caller owns the iterator (never the nodes)

/**
 * @brief Represents the work iterator abstraction in the TIVIDY workflow system.
 */
class WorkIterator {
	public:
		/**
		 * @brief Destroys this object and releases owned resources.
		 */
		virtual ~WorkIterator();
		/**
		 * @brief Performs the first operation.
		 */
		virtual void first() = 0;               // (re)start at the first node
		/**
		 * @brief Performs the next operation.
		 */
		virtual void next() = 0;                // move to the next node
		/**
		 * @brief Performs the current operation.
		 * @return Result produced by the operation.
		 */
		virtual WorkflowNode* current() = 0;    // nullptr when isDone()
		/**
		 * @brief Reports whether the requested condition is satisfied.
		 * @return Result produced by the operation.
		 */
		virtual bool isDone() = 0;              // true when nothing is left
};

#endif // WORKITERATOR_H
