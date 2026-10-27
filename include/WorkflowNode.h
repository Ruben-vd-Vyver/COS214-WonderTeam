#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

class WorkflowNode {
	protected:
		std::string id_;
		std::string name_;
		explicit WorkflowNode(std::string name);
		WorkflowNode(std::string id, std::string name);
	public:
		virtual ~WorkflowNode() = default;
		WorkflowNode(const WorkflowNode&) =delete;
		WorkflowNode& operator=(const WorkflowNode&) = delete;
		const std::string& getId() const;
		std::string getName() const;
		virtual WorkflowNode* clone() const=0;
		virtual void activate() = 0;
		virtual bool isComplete() const = 0;
		virtual WorkItem* find(const std::string& id) = 0;
		virtual WorkflowIterator* createIterator() = 0;
};
