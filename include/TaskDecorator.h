#ifndef TASKDECORATOR_H
#define TASKDECORATOR_H

#include "Task.h"

class TaskDecorator : public Task {
	protected:
		std::shared_ptr<Task> inner_;

		// [Prototype] deep copy: the wrapped task is cloned as well, so a cloned decorator chain never shares its inner task.
		TaskDecorator(const TaskDecorator& other);
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

		// [Prototype] pure here so every concrete decorator MUST override it. 
		// Otherwise it would inherit Task::clone() and silently copy only the Task part (slicing off the decorator).
		Task* clone() const override = 0;
};

#endif // TASKDECORATOR_H
