#ifndef TIVIDY_TUI_OBSERVER_H
#define TIVIDY_TUI_OBSERVER_H

#include <cstddef>
#include <mutex>
#include <string>
#include <vector>

#include "WorkflowObserver.h"

// Receives backend events synchronously and buffers plain text for the UI.
// It never calls ncurses: observer callbacks may occur during engine commands.
class TuiObserver : public WorkflowObserver
{
public:
    explicit TuiObserver(std::size_t capacity = 250);
    void onEvent(const WorkflowEvent &event) override;
    std::vector<std::string> takePendingEvents();

private:
    std::size_t capacity_;
    std::mutex mutex_;
    std::vector<std::string> pending_;
};

#endif // TIVIDY_TUI_OBSERVER_H
