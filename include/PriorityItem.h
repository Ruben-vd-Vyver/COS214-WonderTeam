#include "WorkItemDecorator.h"

class PriorityItem : public WorkItemDecorator {
	int priority_;
public:
	PriorityItem(WorkItem* inner, int priority);
	int getPriority() const override;
	void setPriority(int p);
	void assign(Participant* p) override;
	WorkItem* clone() const override;
};
