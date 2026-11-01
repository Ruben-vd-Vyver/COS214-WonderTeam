#ifndef WORKFLOWNODE_H
#define WORKFLOWNODE_H

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
/**
 * @brief Represents the work item abstraction in the TIVIDY workflow system.
 */
class WorkItem;
/**
 * @brief Represents the workflow iterator abstraction in the TIVIDY workflow system.
 */
class WorkflowIterator;
/**
 * @brief Represents the workflow subject abstraction in the TIVIDY workflow system.
 */
class WorkflowSubject;

/**
 * @brief Represents the workflow node abstraction in the TIVIDY workflow system.
 */
class WorkflowNode {
	protected:
		std::string id_;
		std::string name_;
		/**
		 * @brief Constructs an object of this type.
		 * @param id Value or object supplied by the caller.
		 * @param name Value or object supplied by the caller.
		 */
		WorkflowNode(std::string id, std::string name);
	public:
		/**
		 * @brief Destroys this object and releases owned resources.
		 */
		virtual ~WorkflowNode() = default;
		/**
		 * @brief Constructs an object of this type.
		 */
		WorkflowNode(const WorkflowNode&) =delete;
		WorkflowNode& operator=(const WorkflowNode&) = delete;

		/**
		 * @brief Returns the id.
		 * @return Result produced by the operation.
		 */
		const std::string& getId() const;
		/**
		 * @brief Returns the name.
		 * @return Result produced by the operation.
		 */
		std::string getName() const;

		/**
		 * @brief Performs the clone operation.
		 * @return Result produced by the operation.
		 */
		virtual WorkflowNode* clone() const=0;
		/**
		 * @brief Performs the activate operation.
		 */
		virtual void activate() = 0;
		/**
		 * @brief Reports whether the requested condition is satisfied.
		 * @return Result produced by the operation.
		 */
		virtual bool isComplete() const = 0;
		/**
		 * @brief Performs the bind operation.
		 * @param s Value or object supplied by the caller.
		 */
		virtual void bind(WorkflowSubject* s) = 0;
		/**
		 * @brief Performs the find operation.
		 * @param id Value or object supplied by the caller.
		 * @return Result produced by the operation.
		 */
		virtual WorkItem* find(const std::string& id) = 0;
		/**
		 * @brief Creates the iterator.
		 * @return Result produced by the operation.
		 */
		virtual WorkflowIterator* createIterator() = 0;

		/**
		 * @brief Performs the child count operation.
		 * @return Result produced by the operation.
		 */
		virtual int childCount() const;
		/**
		 * @brief Performs the child operation.
		 * @param i Value or object supplied by the caller.
		 * @return Result produced by the operation.
		 */
		virtual WorkflowNode* child(int i) const;
		/**
		 * @brief Performs the as work item operation.
		 * @return Result produced by the operation.
		 */
		virtual WorkItem* asWorkItem();
};

#endif
