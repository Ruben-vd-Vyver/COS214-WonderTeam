#ifndef TEST_BUILD_HELPERS_H
#define TEST_BUILD_HELPERS_H

// Helpers for the Builder tests: a builder that only records what it is told, and a cerr catcher.

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "test_tree_helpers.h"
#include "WorkflowBuilder.h"
#include "WorkflowDirector.h"

namespace testutil {

// A WorkflowBuilder that builds nothing and logs every call. Proves the Director only needs the
// abstract builder interface, and lets us check exactly which calls a script produces.
class RecordingBuilder : public WorkflowBuilder {
public:
	std::vector<std::string> calls;
	void reset(const std::string& name, int version) override {
		calls.push_back("reset " + name + " " + std::to_string(version));
	}
	void beginStage(const std::string& name, bool parallel) override {
		calls.push_back("beginStage " + name + (parallel ? " parallel" : " sequential"));
	}
	void endStage() override { calls.push_back("endStage"); }
	void addTask(const std::string& name, const std::string& role) override {
		calls.push_back("addTask " + name + " | " + role);
	}
	void addCustomItem(std::shared_ptr<WorkItem>) override { calls.push_back("addCustomItem"); }
	void requireApproval(const std::string& trigger, ApprovalHandler*) override {
		calls.push_back("requireApproval " + trigger);
	}
	WorkflowDefinition* getResult() override { calls.push_back("getResult"); return nullptr; }
};

// Captures everything written to std::cerr while it is alive, so failing scripts don't flood the test output.
class CerrCapture {
	std::ostringstream buffer_;
	std::streambuf* old_;
public:
	CerrCapture() : old_(std::cerr.rdbuf(buffer_.rdbuf())) {}
	~CerrCapture() { std::cerr.rdbuf(old_); }
	std::string str() const { return buffer_.str(); }
};

// Runs a script through a fresh Director into the given builder. Returns the director's verdict.
inline bool runScript(const std::string& script, WorkflowBuilder& builder) {
	std::istringstream in(script);
	WorkflowDirector director(in);
	return director.construct(builder);
}

inline bool contains(const std::string& haystack, const std::string& needle) {
	return haystack.find(needle) != std::string::npos;
}

} // namespace testutil

#endif
