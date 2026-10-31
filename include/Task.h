#ifndef TASK_H
#define TASK_H

#include <map>
#include <string>
#include <vector>

class ApprovalHandler;

// Task (from the class diagram): the *description* of a piece of work in a
// workflow definition, e.g. "Triage, needs role nurse, needs approval when
// completed". It holds no runtime state; each running copy is a
// BasicWorkItem that points at this spec.
class Task {
	private:
		std::string name_;
		std::string requiredRole_;
		std::vector<std::string> requiredFields_;
		std::map<std::string, ApprovalHandler*> approvals_; // trigger -> chain (not owned)
	public:
		Task(std::string name, std::string role);
		const std::string& getName() const;
		const std::string& getRequiredRole() const;
		const std::vector<std::string>& getRequiredFields() const;
		void addRequiredField(const std::string& field);
		void requireApproval(const std::string& trigger, ApprovalHandler* chain);
		bool requiresApproval(const std::string& trigger) const;
		ApprovalHandler* getApprovalChain(const std::string& trigger) const;
};

#endif
