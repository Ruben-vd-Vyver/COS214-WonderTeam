#ifndef SUBPROCESS_H
#define SUBPROCESS_H

#include "CompositeNode.h"

class SubProcess : public CompositeNode {
	int sourceVersion_;
	public:
	SubProcess(std::string id, std::string name, int sourceVersion=1);
	int getSourceVersion() const;
	WorkflowNode* clone() const override;
	void activate() override;
};

#endif
