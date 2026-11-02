#ifndef SUBPROCESS_H
#define SUBPROCESS_H

#include "CompositeNode.h"

/**
 * @brief Represents the sub process abstraction in the TIVIDY workflow system.
 */
class SubProcess : public CompositeNode {
	int sourceVersion_;
	public:
	/**
	 * @brief Constructs an object of this type.
	 * @param id Value or object supplied by the caller.
	 * @param name Value or object supplied by the caller.
	 * @param sourceVersion Value or object supplied by the caller.
	 */
	SubProcess(std::string id, std::string name, int sourceVersion=1);
	/**
	 * @brief Returns the source version.
	 * @return Result produced by the operation.
	 */
	int getSourceVersion() const;
	/**
	 * @brief Performs the clone operation.
	 * @return Result produced by the operation.
	 */
	WorkflowNode* clone() const override;
	/**
	 * @brief Performs the activate operation.
	 */
	void activate() override;
};

#endif
