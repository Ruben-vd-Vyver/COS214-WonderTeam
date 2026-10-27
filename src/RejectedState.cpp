#include "RejectedState.h"
std::string RejectedState::getName() const { return "Rejected"; }
bool RejectedState::hasFailed() const { return true; }
