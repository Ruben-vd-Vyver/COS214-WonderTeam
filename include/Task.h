#include "WorkflowNode.h"
#include "Participant.h"

class Task : public WorkflowNode {
	protected:
		std::string state_ = "Pending";
		bool activated_ = false;
		Participant* assignee_ = nullptr;
		std::string description_;

	public:
		Task(std::string id, std::string name, 
				std::string description = "");
		virtual bool assign(Participant* p);
		virtual bool start();
		virtual bool complete();
		virtual bool reject(const std::string& reason);
		virtual std::string getState() const;
		virtual Participant* getAssignee() const;
		virtual int getPriority() const;
		virtual std::string describe() const;
		void activate() override;
		void isComplete() const override;
		bool hasFailed() const override;
		void print(int indent = 0) const override;
};
