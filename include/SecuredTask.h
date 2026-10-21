#include "TaskDecorator.h"

class SecuredTask : public TaskDecorator {
	int requiredClearance_;
public:
	SecuredTask(std::shared_ptr<Task> inner, int requiredClearance);
	bool assign(Participant* p) override;
	bool complete() override;
	std::string describe() const override;
};
