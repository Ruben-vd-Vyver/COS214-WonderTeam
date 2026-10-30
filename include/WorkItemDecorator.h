#include "WorkItem"

class WorkItemDecorator : public WorkItem {
protected:
	WorkItem* inner_;
public:
	explicit WorkItemDecorator(WorkItem* inner);
	~WorkItemDecorator() override;

	void assign(Participant* p) override;
	void start() override;
	void complete() override;
	void reject(const std::string& reason) override;
	void escalate() override;
	void approve() override;
    	void activate() override;
    	bool isComplete() const override;
    	bool isActive() const override;
    	const Task& getTask() const override;
	std::string getRequiredRole() const override;
    	std::string getStateName() const override;
    	Participant* getAssignee() const override;
    	void setField(const std::string& k, const std::string& value) override;
    	std::string getField(const std::string& key) const override;
    	void bind(WorkflowSubject* s) override;
    	WorkItemMemento* createMemento() const override;
    	void restore(const WorkItemMemento& m) override;
};
