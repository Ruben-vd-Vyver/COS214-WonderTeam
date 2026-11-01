#ifndef WORKFLOWITERATOR_H
#define WORKFLOWITERATOR_H

class WorkflowNode;

class WorkflowIterator {
	public:
		virtual ~WorkflowIterator();
		virtual void first() = 0;               
		virtual void next() = 0;                
		virtual WorkflowNode* current() = 0;    
		virtual bool isDone() = 0;              
};

#endif // WORKFLOWITERATOR_H
