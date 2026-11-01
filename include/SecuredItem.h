#ifndef SECUREDITEM_H
#define SECUREDITEM_H

#include <vector>
#include "WorkItemDecorator.h"

/**
 * @brief Represents the secured item abstraction in the TIVIDY workflow system.
 */
class SecuredItem : public WorkItemDecorator {
    	std::vector<std::string> permittedRoles_;
    	/**
    	 * @brief Performs the permits operation.
    	 * @param p Value or object supplied by the caller.
    	 * @return Result produced by the operation.
    	 */
    	bool permits(const Participant* p) const;
public:
    	/**
    	 * @brief Constructs an object of this type.
    	 * @param inner Value or object supplied by the caller.
    	 * @param permittedRoles Value or object supplied by the caller.
    	 */
    	SecuredItem(WorkItem* inner, std::vector<std::string> permittedRoles);
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
    	 * @brief Performs the clone operation.
    	 * @return Result produced by the operation.
    	 */
    	WorkItem* clone() const override;
};

#endif
