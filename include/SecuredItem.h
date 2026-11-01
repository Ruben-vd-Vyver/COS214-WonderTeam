#ifndef SECUREDITEM_H
#define SECUREDITEM_H

#include <vector>
#include "WorkItemDecorator.h"

class SecuredItem : public WorkItemDecorator {
    	std::vector<std::string> permittedRoles_;
    	bool permits(const Participant* p) const;
public:
    	SecuredItem(WorkItem* inner, std::vector<std::string> permittedRoles);
    	void assign(Participant* p) override;
    	void start() override;
    	const std::vector<std::string>& getPermittedRoles() const;
    	WorkItem* clone() const override;
};

#endif
