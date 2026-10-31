#include "CompositeNode.h"

class Stage : public CompositeNode {
	bool parallel_;
	
	protected:
		std::string label() const override;
	public:
		Stage(std::string id, std::string name, bool parallel);
		bool isParallel() const;
		WorkflowNode* clone() const override;
		void activate() override;
};
