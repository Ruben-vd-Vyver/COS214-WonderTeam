#ifndef PRIORITYITEM_H
#define PRIORITYITEM_H

#include "WorkItemDecorator.h"

/**
 * @brief Represents the priority item abstraction in the TIVIDY workflow system.
 */
class PriorityItem : public WorkItemDecorator {
	int priority_;
public:
	/**
	 * @brief Constructs an object of this type.
	 * @param inner Value or object supplied by the caller.
	 * @param priority Value or object supplied by the caller.
	 */
	PriorityItem(WorkItem* inner, int priority);
	/**
	 * @brief Returns the priority.
	 * @return Result produced by the operation.
	 */
	int getPriority() const;
	/**
	 * @brief Sets the priority.
	 * @param p Value or object supplied by the caller.
	 */
	void setPriority(int p);
	/**
	 * @brief Performs the assign operation.
	 * @param p Value or object supplied by the caller.
	 */
	void assign(Participant* p) override;
	/**
	 * @brief Performs the clone operation.
	 * @return Result produced by the operation.
	 */
	WorkItem* clone() const override;
};

#endif
