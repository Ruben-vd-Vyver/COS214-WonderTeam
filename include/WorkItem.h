#ifndef WORKITEM_H
#define WORKITEM_H

#include <memory>
#include <string>
#include "WorkflowNode.h"
#include "WorkItemMemento.h"
#include "Task.h"

/**
 * @brief Represents the participant abstraction in the TIVIDY workflow system.
 */
class Participant;

/**
 * @brief Represents the work item abstraction in the TIVIDY workflow system.
 */
class WorkItem : public WorkflowNode {
	protected:
		/**
		 * @brief Constructs an object of this type.
		 * @param id Value or object supplied by the caller.
		 * @param name Value or object supplied by the caller.
		 */
		WorkItem(std::string id, std::string name);
	public:
		/**
		 * @brief Performs the assign operation.
		 * @param p Value or object supplied by the caller.
		 */
		virtual void assign(Participant* p) = 0;
		/**
		 * @brief Starts the requested workflow operation.
		 */
		virtual void start() = 0;
		/**
		 * @brief Completes the requested workflow operation.
		 */
		virtual void complete() =0;
		/**
		 * @brief Rejects the operation and records the rejection reason.
		 * @param reason Value or object supplied by the caller.
		 */
		virtual void reject(const std::string& reason) =0;
		/**
		 * @brief Performs the escalate operation.
		 */
		virtual void escalate() = 0;
		/**
		 * @brief Performs the approve operation.
		 */
		virtual void approve() = 0;
		/**
		 * @brief Reports whether the requested condition is satisfied.
		 * @return Result produced by the operation.
		 */
		virtual bool isActive() const = 0;
		/**
		 * @brief Returns the task.
		 * @return Result produced by the operation.
		 */
		virtual const Task& getTask() const = 0;
		/**
		 * @brief Returns the required role.
		 * @return Result produced by the operation.
		 */
		virtual std::string getRequiredRole() const =0;
		/**
		 * @brief Returns the state name.
		 * @return Result produced by the operation.
		 */
		virtual std::string getStateName() const=0;
		/**
		 * @brief Returns the assignee.
		 * @return Result produced by the operation.
		 */
		virtual Participant* getAssignee() const =0;
		/**
		 * @brief Sets the field.
		 * @param key Value or object supplied by the caller.
		 * @param value Value or object supplied by the caller.
		 */
		virtual void setField(const std::string& key,
				const std::string& value) =0;
		/**
		 * @brief Returns the field.
		 * @param key Value or object supplied by the caller.
		 * @return Result produced by the operation.
		 */
		virtual std::string getField(const std::string& key) const=0;
		/**
		 * @brief Creates the memento.
		 * @return Result produced by the operation.
		 */
		virtual std::unique_ptr<WorkItemMemento> createMemento() const=0;
		/**
		 * @brief Performs the restore operation.
		 * @param m Value or object supplied by the caller.
		 */
		virtual void restore(const WorkItemMemento& m)=0;
		/**
		 * @brief Performs the clone operation.
		 * @return Result produced by the operation.
		 */
		WorkItem* clone() const override =0;

		//Leaf behaviour shared by all work items
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
		WorkflowIterator* createIterator() override; //NUllIterator
		/**
		 * @brief Performs the as work item operation.
		 * @return Result produced by the operation.
		 */
		WorkItem* asWorkItem() override;
};

#endif
