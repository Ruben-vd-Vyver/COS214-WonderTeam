#include "WorkItem.h"
#include "NullIterator.h"

WorkItem::WorkItem(std::string name) : WorkflowNode(std::move(name)) {}

WorkItem::WorkItem(std::string id, std::string name) : 
	WorkflowNode(std::move(id), std::move(name)) {}

WorkItem* WorkItem::find(const std::string& id) {
	return id == id_ ? this : nullptr;}

WorkflowIterator* WorkItem::createIterator() {
	return new NullIterator();
}
