#include "PriorityItem.h"

PriorityItem::PriorityItem(WorkItem* inner, int priority) 
	: WorkItemDecorator(inner), priority_(priority) {}

int PriorityItem:getPriority() const {
	return priority_;
}

void PriorityItem::setPriority(int p) {
	priority_ =p;
}

void PriorityItem::assign(Participant* p) {
	WorkItemDecorator::assign(p);
	setField("priority", std::to_string(priority_));
}

WorkItem* PriorityItem::clone() const {
	return new PriorityItem(inner_->clone(), priority_);
}
