#ifndef WORKITERATOR_H
#define WORKITERATOR_H

class WorkflowNode;

// Iterator pattern: abstract Iterator
// Walks the nodes *below* a WorkflowNode without the caller needing to know whether it is a Stage, SubProcess or a single Task.
// Usage:
//   WorkIterator* it = node->createIterator();
//   for (it->first(); !it->isDone(); it->next()) {
//       WorkflowNode* n = it->current();
//       ...
//   }
//   delete it;   // the caller owns the iterator (never the nodes)

class WorkIterator {
	public:
		virtual ~WorkIterator();
		virtual void first() = 0;               // (re)start at the first node
		virtual void next() = 0;                // move to the next node
		virtual WorkflowNode* current() = 0;    // nullptr when isDone()
		virtual bool isDone() = 0;              // true when nothing is left
};

#endif // WORKITERATOR_H
