#ifndef VALIDATEDITEM_H
#define VALIDATEDITEM_H

#include "WorkItemDecorator.h"

/**
 * @brief Represents the validated item abstraction in the TIVIDY workflow system.
 */
class ValidatedItem : public WorkItemDecorator {
public:
	/**
	 * @brief Constructs an object of this type.
	 * @param inner Value or object supplied by the caller.
	 */
	explicit ValidatedItem(WorkItem* inner);
    	/**
    	 * @brief Performs the missing fields operation.
    	 * @return Result produced by the operation.
    	 */
    	std::vector<std::string> missingFields() const;
    	/**
    	 * @brief Completes the requested workflow operation.
    	 */
    	void complete() override;
    	/**
    	 * @brief Performs the clone operation.
    	 * @return Result produced by the operation.
    	 */
    	WorkItem* clone() const override;
};

#endif
