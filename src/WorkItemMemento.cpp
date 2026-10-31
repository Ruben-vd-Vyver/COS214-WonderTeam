#include "WorkItemMemento.h"
#include "WorkItemState.h"
#include <utility>

WorkItemMemento::WorkItemMemento(WorkItemState* state, Participant* assignee,
                                 std::map<std::string, std::string> data)
    : savedState_(state), assignee_(assignee), payload_(std::move(data)) {}

WorkItemMemento::~WorkItemMemento() = default;
