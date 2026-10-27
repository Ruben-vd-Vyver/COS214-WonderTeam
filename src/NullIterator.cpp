#include "NullIterator.h"

void NullIterator::first() {}

void NullIterator::next() {}

WorkflowNode* NullIterator::current() { return nullptr; }

bool NullIterator::isDone() { return true; }
