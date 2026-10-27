#ifndef PRIORITYTASK_H
#define PRIORITYTASK_H

#include "TaskDecorator.h"

class PriorityTask : public TaskDecorator {
	int priority_;
protected:
	// [Prototype]
	PriorityTask(const PriorityTask& other) = default;
public:
	PriorityTask(std::shared_ptr<Task> inner, int priority);
	int getPriority() const override;
	void setPriority(int p);
	std::string describe() const override;

	// [Prototype]
	PriorityTask* clone() const override;
};

#endif // PRIORITYTASK_H
