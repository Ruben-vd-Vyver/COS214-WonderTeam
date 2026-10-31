#ifndef NULLITERATOR_H
#define NULLITERATOR_H

#include "WorkflowIterator.h"

class NullIterator : public WorkflowIterator {
	public:
		void first() override;
		void next() override;
		WorkflowNode* current() override;
		bool isDone() override;
};

#endif // NULLITERATOR_H
