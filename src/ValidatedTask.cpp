#include "ValidatedTask.h"

ValidatedTask::ValidatedTask(std::shared_ptr<Task> inner, std::set<std::string> requiredFields)
    : TaskDecorator(std::move(inner)), required_(std::move(requiredFields)) {}

void ValidatedTask::setField(const std::string& key, const std::string& value) {
	fields_[key] = value; }

std::set<std::string> ValidatedTask::missingFields() const {
	std::set<std::string> missing;
	for (const auto& r : required_) {
        	auto it = fields_.find(r);
        	if (it == fields_.end() ||
				it->second.empty()) missing.insert(r);
	}
	return missing;
}

bool ValidatedTask::complete() {
	//required fields missing
	if (!missingFields().empty()) return false;   
	return TaskDecorator::complete();
}

std::string ValidatedTask::describe() const {
	return TaskDecorator::describe() +
		" [validated, missing " +
		std::to_string(missingFields().size()) + "]";
}

// [Prototype]
ValidatedTask* ValidatedTask::clone() const {
	return new ValidatedTask(*this);
}
