#ifndef WORKITEMDECORATOR_H
#define WORKITEMDECORATOR_H

#include "WorkItem.h"

/**
 * @brief Represents the work item decorator abstraction in the TIVIDY workflow system.
 */
class WorkItemDecorator : public WorkItem {
protected:
	WorkItem* inner_;
public:
	/**
	 * @brief Constructs an object of this type.
	 * @param inner Value or object supplied by the caller.
	 */
	explicit WorkItemDecorator(WorkItem* inner);
	/**
	 * @brief Destroys this object and releases owned resources.
	 */
	~WorkItemDecorator() override;

	/**
	 * @brief Performs the assign operation.
	 * @param p Value or object supplied by the caller.
	 */
	void assign(Participant* p) override;
	/**
	 * @brief Starts the requested workflow operation.
	 */
	void start() override;
	/**
	 * @brief Completes the requested workflow operation.
	 */
	void complete() override;
	/**
	 * @brief Rejects the operation and records the rejection reason.
	 * @param reason Value or object supplied by the caller.
	 */
	void reject(const std::string& reason) override;
	/**
	 * @brief Performs the escalate operation.
	 */
	void escalate() override;
	/**
	 * @brief Performs the approve operation.
	 */
	void approve() override;
    	/**
    	 * @brief Performs the activate operation.
    	 */
    	void activate() override;
    	/**
    	 * @brief Reports whether the requested condition is satisfied.
    	 * @return Result produced by the operation.
    	 */
    	bool isComplete() const override;
    	/**
    	 * @brief Reports whether the requested condition is satisfied.
    	 * @return Result produced by the operation.
    	 */
    	bool isActive() const override;
    	/**
    	 * @brief Returns the task.
    	 * @return Result produced by the operation.
    	 */
    	const Task& getTask() const override;
	/**
	 * @brief Returns the required role.
	 * @return Result produced by the operation.
	 */
	std::string getRequiredRole() const override;
    	/**
    	 * @brief Returns the state name.
    	 * @return Result produced by the operation.
    	 */
    	std::string getStateName() const override;
    	/**
    	 * @brief Returns the assignee.
    	 * @return Result produced by the operation.
    	 */
    	Participant* getAssignee() const override;
    	/**
    	 * @brief Sets the field.
    	 * @param k Value or object supplied by the caller.
    	 * @param value Value or object supplied by the caller.
    	 */
    	void setField(const std::string& k, const std::string& value) override;
    	/**
    	 * @brief Returns the field.
    	 * @param key Value or object supplied by the caller.
    	 * @return Result produced by the operation.
    	 */
    	std::string getField(const std::string& key) const override;
    	/**
    	 * @brief Performs the bind operation.
    	 * @param s Value or object supplied by the caller.
    	 */
    	void bind(WorkflowSubject* s) override;
    	/**
    	 * @brief Creates the memento.
    	 * @return Result produced by the operation.
    	 */
    	std::unique_ptr<WorkItemMemento> createMemento() const override;
    	/**
    	 * @brief Performs the restore operation.
    	 * @param m Value or object supplied by the caller.
    	 */
    	void restore(const WorkItemMemento& m) override;
};

#endif
