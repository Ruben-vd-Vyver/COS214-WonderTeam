#include "ApprovalService.h"

#include <iostream>

void ApprovalService::onEvent( const WorkflowEvent& event) {
    const std::string type = event.getType();

    if (type == "ApprovalRequested" ||type == "ApprovalApproved" ||type == "ApprovalRejected") {
        runApproval(event);
    }
}

void ApprovalService::runApproval(const WorkflowEvent& event) {
    std::string message = event.getType() + " | " + event.getDetail();
    approvalEvents.push_back(message);
    std::cout << "[ApprovalService] " << message << std::endl;
}

const std::vector<std::string>& ApprovalService::getApprovalEvents() const {
    return approvalEvents;
}

void ApprovalService::printApprovalEvents() const {
    for (std::vector<std::string>::const_iterator it = approvalEvents.begin(); it != approvalEvents.end(); ++it) {
        std::cout << *it << std::endl;
    }
}