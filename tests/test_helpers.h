#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

// Shared setup for the pattern tests.

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "DefinitionBuilder.h"
#include "WorkflowDirector.h"
#include "WorkflowDefinition.h"
#include "WorkflowNode.h"
#include "WorkflowIterator.h"
#include "Participant.h"
#include "Task.h"

namespace testutil {

// A small hospital admission workflow in the Director's script format:
//   item-0 Triage, stage-1 Tests (parallel: item-1 Blood test, item-2 X-ray), item-3 Discharge
inline std::string admissionScript() {
	return
		"# hospital admission\n"
		"task  | Triage     | nurse\n"
		"stage | Tests      | parallel\n"
		"task  | Blood test | nurse\n"
		"task  | X-ray      | radiographer\n"
		"end\n"
		"task  | Discharge  | doctor\n";
}

// Builds a definition from a script with the real Director + Builder.
// Returns nullptr if the Director rejects the script. Caller owns the result.
inline WorkflowDefinition* buildDefinition(const std::string& script) {
	std::istringstream in(script);
	WorkflowDirector director(in);
	DefinitionBuilder builder;
	builder.reset("Admission", 1);
	if (!director.construct(builder)) return nullptr;
	return builder.getResult();
}

// Ids of every node below `node`, in iteration order, space separated.
inline std::string visitOrder(WorkflowNode* node) {
	std::string out;
	WorkflowIterator* it = node->createIterator();
	for (it->first(); !it->isDone(); it->next())
		out += (out.empty() ? "" : " ") + it->current()->getId();
	delete it;
	return out;
}

inline std::shared_ptr<Task> spec(const std::string& name, const std::string& role) {
	return std::shared_ptr<Task>(new Task(name, role));
}

inline std::vector<std::string> roles(const std::string& role) {
	return std::vector<std::string>(1, role);
}

} // namespace testutil

#endif
