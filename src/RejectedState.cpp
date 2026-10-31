#include "RejectedState.h"

std::string RejectedState::getName() const { return "Rejected"; }
WorkItemState *RejectedState::clone() const { return new RejectedState(); }
