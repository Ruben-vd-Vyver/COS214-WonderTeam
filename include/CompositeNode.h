#ifndef COMPOSITENODE_H
#define COMPOSITENODE_H

#include "WorkflowNode.h"

/**
 * @brief Represents the composite node abstraction in the TIVIDY workflow system.
 */
class CompositeNode : public WorkflowNode {
	protected:
		std::vector<WorkflowNode*> children_;
		WorkflowSubject* subject_ = nullptr;
		/**
		 * @brief Constructs an object of this type.
		 * @param id Value or object supplied by the caller.
		 * @param name Value or object supplied by the caller.
		 */
		CompositeNode(std::string id,std::string name);
		/**
		 * @brief Performs the clone children into operation.
		 * @param target Value or object supplied by the caller.
		 */
		void cloneChildrenInto(CompositeNode& target) const;

	public:
		/**
		 * @brief Destroys this object and releases owned resources.
		 */
		~CompositeNode() override;
		/**
		 * @brief Performs the add operation.
		 * @param node Value or object supplied by the caller.
		 */
		void add(WorkflowNode* node);
		/**
		 * @brief Performs the remove operation.
		 * @param node Value or object supplied by the caller.
		 */
		void remove(WorkflowNode* node);
		/**
		 * @brief Reports whether the requested condition is satisfied.
		 * @return Result produced by the operation.
		 */
		bool isComplete() const override;
		/**
		 * @brief Performs the child count operation.
		 * @return Result produced by the operation.
		 */
		int childCount() const override;
		/**
		 * @brief Performs the child operation.
		 * @param i Value or object supplied by the caller.
		 * @return Result produced by the operation.
		 */
		WorkflowNode* child(int i) const override;
		/**
		 * @brief Performs the bind operation.
		 * @param s Value or object supplied by the caller.
		 */
		void bind(WorkflowSubject* s) override;
		/**
		 * @brief Performs the find operation.
		 * @param id Value or object supplied by the caller.
		 * @return Result produced by the operation.
		 */
		WorkItem* find(const std::string& id) override;
		/**
		 * @brief Creates the iterator.
		 * @return Result produced by the operation.
		 */
		WorkflowIterator* createIterator() override;
};

#endif
