#ifndef TEST_TREE_HELPERS_H
#define TEST_TREE_HELPERS_H

// Extra helpers for the thorough Composite / Decorator tests.
// Trees are built by hand (no Director/Builder), so these tests only fail when the pattern classes themselves break.

#include <memory>
#include <string>
#include <vector>

#include "test_helpers.h"
#include "BasicWorkItem.h"
#include "Stage.h"
#include "SubProcess.h"
#include "WorkItem.h"
#include "WorkflowEvent.h"
#include "WorkflowObserver.h"
#include "WorkflowSubject.h"

namespace testutil {

inline std::shared_ptr<BasicWorkItem> makeItem(const std::string& id, const std::string& role = "nurse") {
	return std::make_shared<BasicWorkItem>(id, spec("Task " + id, role));
}

inline std::shared_ptr<Stage> makeStage(const std::string& id, bool parallel = false) {
	return std::make_shared<Stage>(id, "Stage " + id, parallel);
}

inline std::shared_ptr<SubProcess> makeSub(const std::string& id, int version = 1) {
	return std::make_shared<SubProcess>(id, "Sub " + id, version);
}

// Takes an item through its whole happy path: Pending -> Available -> Assigned -> InProgress -> Completed.
inline void work(WorkItem& w, Participant& who) {
	w.activate();
	w.assign(&who);
	w.start();
	w.complete();
}

// Records every event a WorkflowSubject publishes.
struct Recorder : WorkflowObserver {
	std::vector<std::string> types;
	std::vector<std::string> details;
	std::vector<std::string> itemIds;
	void onEvent(const WorkflowEvent& e) override {
		types.push_back(e.getType());
		details.push_back(e.getDetail());
		itemIds.push_back(e.getItem() ? e.getItem()->getId() : std::string());
	}
	int count(const std::string& type) const {
		int n = 0;
		for (size_t i = 0; i < types.size(); ++i) if (types[i] == type) ++n;
		return n;
	}
};

// A BasicWorkItem that reports its own destruction, to prove who owns what.
struct SpyItem : BasicWorkItem {
	int* destroyed;
	SpyItem(const std::string& id, std::shared_ptr<const Task> t, int* counter)
		: BasicWorkItem(id, t), destroyed(counter) {}
	~SpyItem() override { ++*destroyed; }
};

} // namespace testutil

#endif
