#ifndef VALIDATEDTASK_H
#define VALIDATEDTASK_H

#include <map>
#include <set>
#include "TaskDecorator.h"

class ValidatedTask : public TaskDecorator {
    std::set<std::string> required_;
    std::map<std::string, std::string> fields_;
protected:
    // [Prototype] the field values are copied along with the decorator
    ValidatedTask(const ValidatedTask& other) = default;
public:
    ValidatedTask(std::shared_ptr<Task> inner, std::set<std::string> requiredFields);
    void setField(const std::string& key, const std::string& value);
    std::set<std::string> missingFields() const;
    bool complete() override;
    std::string describe() const override;

    // [Prototype]
    ValidatedTask* clone() const override;
};

#endif // VALIDATEDTASK_H
