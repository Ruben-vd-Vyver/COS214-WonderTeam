#include "WorkItemDecorator.h"

class ValidatedItem : public WorkItemDecorator {
public:
	explicit ValidatedItem(WorkItem* inner);
    	std::vector<std::string> missingFields() const;
    	void complete() override;
    	WorkItem* clone() const override;
};
