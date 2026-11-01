#ifndef STAGE_H
#define STAGE_H

#include "CompositeNode.h"

/**
 * @brief Represents the stage abstraction in the TIVIDY workflow system.
 */
class Stage : public CompositeNode {
	bool parallel_;
	
	public:
		/**
		 * @brief Constructs an object of this type.
		 * @param id Value or object supplied by the caller.
		 * @param name Value or object supplied by the caller.
		 * @param parallel Value or object supplied by the caller.
		 */
		Stage(std::string id, std::string name, bool parallel);
		/**
		 * @brief Reports whether the requested condition is satisfied.
		 * @return Result produced by the operation.
		 */
		bool isParallel() const;
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
