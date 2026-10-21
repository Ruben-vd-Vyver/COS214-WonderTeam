#include "TaskDecorator.h"

class PriorityTask : public TaskDecorator {
	int priority_;
public:
	PriorityTask(std::shared_ptr<Task> inner, int priority);
	int getPriority() const override;
	void setPriority(int p);
	std::string describe() const override;
};
