#include "SecuredItem.h"
#include <algorithm>
#include <stdexcept>
#include "Participant.h"

SecuredItem::SecuredItem(WorkItem* inner, std::vector<std::string> permittedRoles)
    : WorkItemDecorator(inner), permittedRoles_(std::move(permittedRoles)) {}

bool SecuredItem::permits(const Participant* p) const {
	if (!p) return false;
	for (std::size_t i = 0; i < permittedRoles_.size(); ++i)
		if (p->hasRole(permittedRoles_[i])) return true;
	return false;
}

void SecuredItem::assign(Participant* p) {
    	if (p && !permits(p)) throw std::runtime_error
		("'" + name_ + "' is secured: '" + p->getName() +
		 "' has no permitted role");
    WorkItemDecorator::assign(p);
}

void SecuredItem::start() {
	    if (!permits(getAssignee())) throw std::runtime_error(
			    "'" + name_ + "' is secured: no permitted assignee");
    WorkItemDecorator::start();
}

WorkItem* SecuredItem::clone() const { 
	return new SecuredItem(inner_->clone(), permittedRoles_); 
}
