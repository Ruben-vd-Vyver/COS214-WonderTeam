#include "TuiObserver.h"

#include <ctime>
#include <iomanip>
#include <sstream>

#include "WorkItem.h"

TuiObserver::TuiObserver(std::size_t capacity)
    : capacity_(capacity)
{
}

void TuiObserver::onEvent(const WorkflowEvent &event)
{
    std::lock_guard<std::mutex> lock(mutex_);

    const std::time_t timestamp = event.getTimestamp();
    const std::tm *local = std::localtime(&timestamp);
    std::ostringstream line;
    if (local)
        line << std::put_time(local, "%H:%M:%S") << " ";

    std::string displayType = event.getType();
    if (displayType == "state.changed")
    {
        if (event.getDetail() == "Assigned") displayType = "TaskAssigned";
        else if (event.getDetail() == "InProgress") displayType = "TaskStarted";
        else if (event.getDetail() == "Completed") displayType = "TaskCompleted";
        else if (event.getDetail() == "Rejected") displayType = "TaskRejected";
        else if (event.getDetail() == "AwaitingApproval") displayType = "ApprovalRequested";
    }

    line << displayType;
    if (event.getItem())
        line << " [" << event.getItem()->getId() << ": " << event.getItem()->getName() << "]";
    if (!event.getDetail().empty())
        line << " - " << event.getDetail();

    pending_.push_back(line.str());
    if (capacity_ > 0 && pending_.size() > capacity_)
        pending_.erase(pending_.begin(), pending_.begin() + (pending_.size() - capacity_));
}

std::vector<std::string> TuiObserver::takePendingEvents()
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> result;
    result.swap(pending_);
    return result;
}
