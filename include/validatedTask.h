#include <map>
#include <set>
#include "TaskDecorator.h"

class ValidatedTask : public TaskDecorator {
    std::set<std::string> required_;
    std::map<std::string, std::string> fields_;
public:
    ValidatedTask(std::shared_ptr<Task> inner,
		    std::set<std::string> requiredFields);
    void setField(const std::string& key, const std::string& value);
    std::set<std::string> missingFields() const;
    bool complete() override;
    std::string describe() const override;
};
