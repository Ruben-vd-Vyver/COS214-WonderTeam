#ifndef NULLITERATOR_H
#define NULLITERATOR_H

#include "WorkIterator.h"

// Iterator pattern: ConcreteIterator for leaves (Tasks).
// A leaf has nothing below it, so this iterator is always done. 
//It lets callers loop over any node the same way without checking for leaves.
class NullIterator : public WorkIterator {
	public:
		void first() override;
		void next() override;
		WorkflowNode* current() override;
		bool isDone() override;
};

#endif // NULLITERATOR_H
