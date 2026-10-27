#include "WorkflowNode.h"
#include "Participant.h"
#include <algorithm>

class Task : public WorkflowNode {
	protected:
		std::string state_ = "Pending";
		bool activated_ = false;
		std::vector<Participant*> assignees_;
		std::string description_;

	public:
		Task(std::string id, std::string name, 
				std::string description = "");
		virtual bool assign(Participant* p);
		virtual bool unassign(Participant* p);
		virtual bool start();
		virtual bool complete();
		virtual bool reject(const std::string& reason);
		virtual std::string getState() const;
		virtual std::vector<Participant*> getAssignees() const;
		virtual int getPriority() const;
		virtual std::string describe() const;
		void activate() override;
		bool isComplete() const override;
		bool hasFailed() const override;
		void print(int indent = 0) const override;
};
