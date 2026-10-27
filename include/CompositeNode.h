#include "WorkflowNode.h"

class CompositeNode : public WorkflowNode {
	protected:
		std::vector<std::shared_ptr<WorkflowNode>> children_;
		virtual std::string label() const;

	public:
		CompositeNode(std::string id, std::string name);
		void add(std::shared_ptr<WorkflowNode> node) override;
		bool remove(const std::string& id) override;
		std::shared_ptr<WorkflowNode> getChild(
				const std::string& id) const override;
		const std::vector<std::shared_ptr<WorkflowNode>>& getChildren() const override;
		void activate() override;
		bool isComplete() const override;
		bool hasFailed() const override;
		void print(int indent=0)const override;
};
