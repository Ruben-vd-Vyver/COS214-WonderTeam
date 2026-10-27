#include "WorkflowNode.h"

class CompositeNode : public WorkflowNode {
	protected:
		std::vector<std::shared_ptr<WorkflowNode>> children_;
		explicit CompositeNode(std::string name);
		void cloneChildrenInto(CompositeNode& target) const;
		bool contains(const WorkflowNode* node) const;

	public:
		~CompositeNode() override;
		void add(WorkflowNode* node);
		void remove(WorkflowNode* node);
		const std::vector<stdLLshared_ptr<WorkflowNode>> & 
			getChildren() const override;
		bool isComplete() const override;
		WorkItem* find(const std::string& id) override;
		WorkflowIterator* createIterator() override;
};
