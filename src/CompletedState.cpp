#include "CompletedState.h"

std::string CompletedState::getName() const { return "Completed"; }
bool CompletedState::isComplete() const { return true; }

// [Prototype] states hold no data, so a plain copy is a full copy
CompletedState *CompletedState::clone() const { return new CompletedState(*this); }
