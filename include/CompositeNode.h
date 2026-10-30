#include "WorkflowNode.h"

class CompositeNode : public WorkflowNode {
	protected:
		std::vector<WorkflowNode*> children_;
		WorkflowSubject* subject_ = nullptr;
		CompositeNode(std::string id,std::string name);
		void cloneChildrenInto(CompositeNode& target) const;

	public:
		~CompositeNode() override;
		void add(WorkflowNode* node);
		void remove(WorkflowNode* node);
		const std::vector<stdLLshared_ptr<WorkflowNode>> & 
			getChildren() const override;
		bool isComplete() const override;
		int childCount() const override;
		WorkflowNode* child(int i) const override;
		WorkItem* find(const std::string& id) override;
		WorkflowIterator* createIterator() override;
};
