#include "ValidatedItem.h"
#include <stdexcept>

ValidatedItem::ValidatedItem(WorkItem* inner) : WorkItemDecorator(inner) {}

std::vector<std::string> ValidatedItem::missingFields() const {
    	std::vector<std::string> missing;
    	for (const auto& f : getTask().getRequiredFields())
        	if (getField(f).empty()) missing.push_back(f);
    	return missing;
}

void ValidatedItem::complete() {
    	auto missing = missingFields();
    	if (!missing.empty()) {
        	std::string msg = "'" + name_ + "' cannot complete, missing:";
        	for (const auto& m : missing) msg += " " + m;
        	throw std::runtime_error(msg);
    	}
    	WorkItemDecorator::complete();
}

WorkItem* ValidatedItem::clone() const { 
	return new ValidatedItem(inner_->clone());
}

