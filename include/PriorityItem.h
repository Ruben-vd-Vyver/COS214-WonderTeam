#ifndef PRIORITYITEM_H
#define PRIORITYITEM_H

#include "WorkItemDecorator.h"

class PriorityItem : public WorkItemDecorator {
	int priority_;
public:
	PriorityItem(WorkItem* inner, int priority);
	int getPriority() const;
	void setPriority(int p);
	void assign(Participant* p) override;
	WorkItem* clone() const override;
};

#endif
