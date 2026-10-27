#include "RejectedState.h"
std::string RejectedState::getName() const { return "Rejected"; }
bool RejectedState::hasFailed() const { return true; }

// [Prototype] states hold no data, so a plain copy is a full copy
RejectedState *RejectedState::clone() const { return new RejectedState(*this); }
