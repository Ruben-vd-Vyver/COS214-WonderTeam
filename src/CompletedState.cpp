#include "CompletedState.h"

std::string CompletedState::getName() const { return "Completed"; }
bool CompletedState::isComplete() const { return true; }