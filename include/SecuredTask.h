#ifndef SECUREDTASK_H
#define SECUREDTASK_H

#include "TaskDecorator.h"

class SecuredTask : public TaskDecorator {
	int requiredClearance_;
protected:
	// [Prototype]
	SecuredTask(const SecuredTask& other) = default;
public:
	SecuredTask(std::shared_ptr<Task> inner, int requiredClearance);
	bool assign(Participant* p) override;
	bool complete() override;
	std::string describe() const override;

	// [Prototype]
	SecuredTask* clone() const override;
};

#endif // SECUREDTASK_H
