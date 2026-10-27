#include "CompositeNode.h"

class Stage : public CompositeNode {
	bool parallel_;
	
	protected:
		std::string label() const override;
	public:
		explicit Stage(std::string id, std::string name, bool parallel = false);
		bool isParallel() const;
		WorkflowNode* clone() const override;
		void activate() override;
};
