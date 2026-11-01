#include "CompletedState.h"

std::string CompletedState::getName() const { return "Completed"; }
WorkItemState *CompletedState::clone() const { return new CompletedState(); }
bool CompletedState::isComplete() const { return true; }
