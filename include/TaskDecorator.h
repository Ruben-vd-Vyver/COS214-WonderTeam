#include "Task.h"

class TaskDecorator : public Task {
	protected:
		std::shared_ptr<Task> inner_;
	public:
		explicit TaskDecorator(std::shared_ptr<Task> inner);
		bool assign(Participant* p) override;
		bool start() override;
		bool complete() override;
		bool reject(const std::string& reason) override;
		std::string getState() const override;
		Participant* getAssignee() const override;
		int getPriority() const override;
		std::string describe() const override;
		void activate() override;
		bool isComplete() const override;
		bool hasFailed() const override;
};
