#ifndef COMPOSITENODE_H
#define COMPOSITENODE_H

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
		bool isComplete() const override;
		int childCount() const override;
		WorkflowNode* child(int i) const override;
		void bind(WorkflowSubject* s) override;
		WorkItem* find(const std::string& id) override;
		WorkflowIterator* createIterator() override;
};

#endif
