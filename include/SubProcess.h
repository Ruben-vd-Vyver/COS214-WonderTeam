#include "CompositeNode.h"

class SubProcess : public CompositeNode {
	int sourceVersion_;
	public:
	explicit SubProcess(std::string name, int sourceVersion=1);
	int getSourceVersion() const;
	WorkflowNode* clone() const override;
	void activate() override;
};
