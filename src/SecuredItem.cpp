#include "SecuredItem.h"
#include <algorithm>
#include <stdexcept>
#include "Participant.h"

SecuredItem::SecuredItem(WorkItem* inner, std::vector<std::string> permittedRoles)
    : WorkItemDecorator(inner), permittedRoles_(std::move(permittedRoles)) {}

bool SecuredItem::permits(const Participant* p) const {
    	return p && std::find(permittedRoles_.begin(), 
			permittedRoles_.end(), 
			p->getRole()) != permittedRoles_.end();
}

void SecuredItem::assign(Participant* p) {
    	if (p && !permits(p)) throw std::runtime_error
		("'" + name_ + "' is secured: role '" + (p ? p->getRole() :
							 "none") +
		 "' not permitted");
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
