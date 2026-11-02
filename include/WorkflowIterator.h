#ifndef WORKFLOWITERATOR_H
#define WORKFLOWITERATOR_H

/**
 * @brief Represents the workflow node abstraction in the TIVIDY workflow system.
 */
class WorkflowNode;

/**
 * @brief Represents the workflow iterator abstraction in the TIVIDY workflow system.
 */
class WorkflowIterator {
	public:
		/**
		 * @brief Destroys this object and releases owned resources.
		 */
		virtual ~WorkflowIterator();
		/**
		 * @brief Performs the first operation.
		 */
		virtual void first() = 0;               
		/**
		 * @brief Performs the next operation.
		 */
		virtual void next() = 0;                
		/**
		 * @brief Performs the current operation.
		 * @return Result produced by the operation.
		 */
		virtual WorkflowNode* current() = 0;    
		/**
		 * @brief Reports whether the requested condition is satisfied.
		 * @return Result produced by the operation.
		 */
		virtual bool isDone() = 0;              
		/**
		 * @brief Returns the current node's depth relative to the iterator root.
		 * @return The current node's depth, or -1 if there is no current node.
		 */
		virtual int currentDepth() const = 0; // depth of current node relative to iterator root
};

#endif // WORKFLOWITERATOR_H
