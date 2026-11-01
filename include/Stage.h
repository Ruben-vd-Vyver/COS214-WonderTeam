#ifndef STAGE_H
#define STAGE_H

#include "CompositeNode.h"

class Stage : public CompositeNode {
	bool parallel_;
	
	public:
		Stage(std::string id, std::string name, bool parallel);
		bool isParallel() const;
		WorkflowNode* clone() const override;
		void activate() override;
};

#endif
