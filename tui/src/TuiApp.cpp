#include "TuiApp.h"

#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <stdexcept>

#include "Participant.h"

namespace
{
enum ColorPair
{
    CP_HEADER = 1,
    CP_TAB_ACTIVE,
    CP_PENDING,
    CP_ASSIGNED,
    CP_PROGRESS,
    CP_APPROVAL,
    CP_DONE,
    CP_REJECTED,
    CP_SELECTED,
    CP_FOOTER,
    CP_KEY,
    CP_FOLDER
};

const char *TAB_NAMES[] = {
    "Overview", "To Do", "In Progress", "To Approve", "Completed", "All"};

std::vector<std::string> splitRoles(const std::string &input)
{
    std::vector<std::string> roles;
    std::stringstream stream(input);
    std::string role;
    while (std::getline(stream, role, ','))
    {
        const std::size_t first = role.find_first_not_of(" \t");
        if (first == std::string::npos)
            continue;
        const std::size_t last = role.find_last_not_of(" \t");
        roles.push_back(role.substr(first, last - first + 1));
    }
    return roles;
}

std::string joinRoles(const std::vector<std::string> &roles)
{
    std::string result;
    for (std::size_t i = 0; i < roles.size(); ++i)
    {
        if (i)
            result += ", ";
        result += roles[i];
    }
    return result.empty() ? "(no roles)" : result;
}

bool hasRole(const Participant &participant, const std::string &role)
{
    return participant.hasRole(role);
}

bool isToDoState(const std::string &state)
{
    return state == "Pending" || state == "Available" ||
           state == "Escalated" || state == "Assigned";
}

bool stateInTab(int tab, const std::string &state)
{
    // Status tabs are indexed independently from the Overview tab.
    switch (tab)
    {
    case 0: return isToDoState(state);
    case 1: return state == "InProgress";
    case 2: return state == "AwaitingApproval";
    case 3: return state == "Completed";
    default: return true;
    }
}

bool isYes(const std::string &answer)
{
    return answer == "y" || answer == "Y" || answer == "yes" || answer == "YES";
}

int stateColor(const std::string &state)
{
    if (state == "Pending" || state == "Available" || state == "Escalated")
        return CP_PENDING;
    if (state == "Assigned")
        return CP_ASSIGNED;
    if (state == "InProgress")
        return CP_PROGRESS;
    if (state == "AwaitingApproval")
        return CP_APPROVAL;
    if (state == "Completed" || state == "Complete")
        return CP_DONE;
    if (state == "Rejected")
        return CP_REJECTED;
    if (state == "In progress")
        return CP_PROGRESS;
    return CP_FOLDER;
}

std::string displayState(const std::string &state)
{
    if (state == "InProgress") return "In Progress";
    if (state == "AwaitingApproval") return "Awaiting Approval";
    if (state == "Complete") return "Complete";
    return state;
}

std::string assignmentStrategyLabel(const std::string &mode)
{
    if (mode == "role" || mode == "role-based")
        return "Role-based";
    if (mode == "workload" || mode == "least-workload")
        return "Least-workload";
    if (mode == "rule" || mode == "rule-based")
        return "Rule-based";
    return "Not configured";
}

void safePrint(WINDOW *window, int y, int x, int width, const std::string &text)
{
    if (!window || width <= 0 || y < 0 || x < 0 ||
        y >= getmaxy(window) || x >= getmaxx(window))
        return;
    mvwprintw(window, y, x, "%.*s", std::min(width, getmaxx(window) - x), text.c_str());
}
} // namespace

TuiApp::TuiApp(TividyEngine &engine, const std::string &instanceId,
               const std::shared_ptr<TuiObserver> &observer,
               std::vector<std::shared_ptr<Participant> > &participantOwners)
    : engine_(engine), instanceId_(instanceId), observer_(observer),
      participants_(participantOwners), activeParticipant_(0), running_(false),
      cursesActive_(false), mineOnly_(true), activeTab_(0), selectedRow_(0),
      treeOffset_(0)
{
    if (!observer_)
        throw std::invalid_argument("TuiApp requires a live TuiObserver");
}

TuiApp::~TuiApp()
{
    endNcurses();
}

void TuiApp::addParticipant(const std::string &name, int authority,
                            const std::vector<std::string> &roles)
{
    if (name.empty())
        throw std::invalid_argument("Participant name cannot be empty");
    if (roles.empty())
        throw std::invalid_argument("Add at least one participant role");

    std::shared_ptr<Participant> participant(new Participant(name, authority, roles));
    participants_.push_back(participant);
    try
    {
        engine_.registerParticipant(participant.get());
    }
    catch (...)
    {
        participants_.pop_back();
        throw;
    }
    if (participants_.size() == 1)
        activeParticipant_ = 0;
    rebuildVisibleEntries();
}

int TuiApp::run()
{
    initNcurses();
    running_ = true;
    try
    {
        while (running_)
        {
            collectEvents();
            rebuildVisibleEntries();
            render();
            timeout(200); // observer callbacks buffer messages; this only refreshes the display
            const int key = getch();
            if (key != ERR)
                handleInput(key);
        }
    }
    catch (...)
    {
        endNcurses();
        throw;
    }
    endNcurses();
    return 0;
}

void TuiApp::initNcurses()
{
    if (cursesActive_)
        return;
    initscr();
    cursesActive_ = true;
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    if (has_colors())
    {
        start_color();
        use_default_colors();
        init_pair(CP_HEADER, COLOR_WHITE, COLOR_BLUE);
        init_pair(CP_TAB_ACTIVE, COLOR_BLACK, COLOR_CYAN);
        init_pair(CP_PENDING, COLOR_YELLOW, -1);
        init_pair(CP_ASSIGNED, COLOR_CYAN, -1);
        init_pair(CP_PROGRESS, COLOR_BLUE, -1);
        init_pair(CP_APPROVAL, COLOR_YELLOW, -1);
        init_pair(CP_DONE, COLOR_GREEN, -1);
        init_pair(CP_REJECTED, COLOR_MAGENTA, -1);
        init_pair(CP_SELECTED, COLOR_BLACK, COLOR_WHITE);
        init_pair(CP_FOOTER, COLOR_BLACK, COLOR_WHITE);
        init_pair(CP_KEY, COLOR_BLACK, COLOR_YELLOW);
        init_pair(CP_FOLDER, COLOR_CYAN, -1);
    }
    refresh();
}

void TuiApp::endNcurses()
{
    if (!cursesActive_)
        return;
    topWindow_.destroy();
    tabsWindow_.destroy();
    treeWindow_.destroy();
    detailWindow_.destroy();
    logWindow_.destroy();
    footerWindow_.destroy();
    endwin();
    cursesActive_ = false;
}

void TuiApp::resizeWindows()
{
    const int rows = LINES;
    const int cols = COLS;
    if (rows < 22 || cols < 64)
        return;

    const int topHeight = 2;
    const int tabsHeight = 2;
    // One additional detail row is reserved for the active sequential gate.
    const int detailHeight = 7;
    const int logHeight = std::max(3, std::min(5, rows / 6));
    const int footerHeight = 5;
    const int treeHeight = rows - topHeight - tabsHeight - detailHeight - logHeight - footerHeight;

    if (treeHeight < 3)
        return;

    const int treeY = topHeight + tabsHeight;
    const int detailY = treeY + treeHeight;
    const int logY = detailY + detailHeight;
    const int footerY = rows - footerHeight;

    topWindow_.resize(topHeight, cols, 0, 0);
    tabsWindow_.resize(tabsHeight, cols, topHeight, 0);
    treeWindow_.resize(treeHeight, cols, treeY, 0);
    detailWindow_.resize(detailHeight, cols, detailY, 0);
    logWindow_.resize(logHeight, cols, logY, 0);
    footerWindow_.resize(footerHeight, cols, footerY, 0);
}

void TuiApp::render()
{
    if (LINES < 22 || COLS < 64)
    {
        erase();
        mvprintw(0, 0, "TIVIDY needs a terminal of at least 64 columns x 22 rows.");
        mvprintw(1, 0, "Current size: %d x %d. Resize the terminal; press q to quit.", COLS, LINES);
        refresh();
        return;
    }

    resizeWindows();
    if (!topWindow_.get() || !tabsWindow_.get() || !treeWindow_.get() ||
        !detailWindow_.get() || !logWindow_.get() || !footerWindow_.get())
        return;

    // Clear the complete base screen before composing the panes. This also
    // clears any characters left behind if a pane was previously taller or
    // entries were removed from the workflow tree. Panes are queued afterwards
    // and doupdate() commits the combined frame once, avoiding intermediate
    // blank-screen flicker.
    werase(stdscr);
    touchwin(stdscr);
    wnoutrefresh(stdscr);

    renderTop();
    renderTabs();
    renderTree();
    renderDetails();
    renderLog();
    renderFooter();

    // Force each complete pane into the virtual screen on every frame. This is
    // intentional: when filtering, changing tabs, expanding folders, or running
    // commands makes the tree shorter, ncurses must repaint the now-empty rows
    // instead of leaving text from the previous frame visible underneath.
    WINDOW *panes[] = {
        topWindow_.get(), tabsWindow_.get(), treeWindow_.get(),
        detailWindow_.get(), logWindow_.get(), footerWindow_.get()
    };
    for (std::size_t i = 0; i < sizeof(panes) / sizeof(panes[0]); ++i)
    {
        if (panes[i])
        {
            touchwin(panes[i]);
            wnoutrefresh(panes[i]);
        }
    }
    doupdate();
}

void TuiApp::renderTop()
{
    WINDOW *window = topWindow_.get();
    werase(window);
    wbkgd(window, COLOR_PAIR(CP_HEADER));

    std::string workflowName = allEntries_.empty() ? "Workflow" : allEntries_.front().name;
    const std::string title = " TIVIDY | " + workflowName + " | Instance " + instanceId_;
    wattron(window, COLOR_PAIR(CP_HEADER) | A_BOLD);
    safePrint(window, 0, 1, getmaxx(window) - 2, title);
    wattroff(window, COLOR_PAIR(CP_HEADER) | A_BOLD);

    // Keep the active assignment strategy in a dedicated, always-visible
    // header badge. Previously it was appended to the long participant summary
    // and was silently clipped on normal terminal widths.
    Participant *active = activeParticipant();
    std::ostringstream summary;
    if (active)
    {
        int workload = 0;
        for (std::size_t i = 0; i < allEntries_.size(); ++i)
        {
            const TividyEngine::WorkflowEntry &entry = allEntries_[i];
            if (entry.isWorkItem && entry.assignee == active->getName() &&
                entry.state != "Completed" && entry.state != "Rejected")
                ++workload;
        }
        summary << "Active: " << active->getName() << " | Load: " << workload
                << " | Roles: " << joinRoles(active->getRoles());
    }
    else
    {
        summary << "No active participant. Press N to add one.";
    }

    const std::string strategy = "Auto: " +
        assignmentStrategyLabel(engine_.assignmentMode(instanceId_)) + "  [ / ]";
    const int width = getmaxx(window);
    const int strategyX = std::max(2, width - static_cast<int>(strategy.size()) - 2);
    const int summaryWidth = std::max(0, strategyX - 3);

    wattron(window, COLOR_PAIR(CP_HEADER));
    safePrint(window, 1, 2, summaryWidth, summary.str());
    wattroff(window, COLOR_PAIR(CP_HEADER));

    wattron(window, COLOR_PAIR(CP_KEY) | A_BOLD);
    safePrint(window, 1, strategyX, width - strategyX - 1, strategy);
    wattroff(window, COLOR_PAIR(CP_KEY) | A_BOLD);
}

int TuiApp::countTabItems(int tab) const
{
    Participant *active = activeParticipant();
    int count = 0;
    for (std::size_t i = 0; i < allEntries_.size(); ++i)
    {
        const TividyEngine::WorkflowEntry &entry = allEntries_[i];
        if (!entry.isWorkItem)
            continue;
        // Overview is deliberately global. The All tab respects the participant
        // filter and the four status tabs combine status + participant filtering.
        if (tab != 0 && tab != 5 && !stateInTab(tab - 1, entry.state))
            continue;
        if (tab != 0 && mineOnly_ && active && entry.assignee != active->getName() &&
            !(entry.assignee == "unassigned" && hasRole(*active, entry.requiredRole)))
            continue;
        ++count;
    }
    return count;
}

void TuiApp::renderTabs()
{
    WINDOW *window = tabsWindow_.get();
    werase(window);
    const char *compactNames[TAB_COUNT] = {"Overview", "ToDo", "Doing", "Review", "Done", "All"};
    const bool compact = getmaxx(window) < 90;
    const bool hideCounts = getmaxx(window) < 80;
    int x = 2;
    for (int tab = 0; tab < TAB_COUNT; ++tab)
    {
        std::ostringstream label;
        label << " " << (compact ? compactNames[tab] : TAB_NAMES[tab]);
        if (!hideCounts)
            label << " (" << countTabItems(tab) << ")";
        label << " ";
        const std::string text = label.str();
        if (tab == activeTab_)
        {
            wattron(window, COLOR_PAIR(CP_TAB_ACTIVE) | A_BOLD);
            mvwprintw(window, 0, x, "%s", text.c_str());
            wattroff(window, COLOR_PAIR(CP_TAB_ACTIVE) | A_BOLD);
        }
        else
            mvwprintw(window, 0, x, "%s", text.c_str());
        x += static_cast<int>(text.size()) + 1;
    }
    mvwhline(window, 1, 0, ACS_HLINE, getmaxx(window));
}

bool TuiApp::hasChildren(std::size_t index) const
{
    if (index + 1 >= allEntries_.size())
        return false;
    return allEntries_[index + 1].depth > allEntries_[index].depth;
}

int TuiApp::countDescendantItems(std::size_t index, int *completed) const
{
    if (index >= allEntries_.size())
        return 0;
    const int depth = allEntries_[index].depth;
    int total = 0;
    int done = 0;
    for (std::size_t i = index + 1; i < allEntries_.size() && allEntries_[i].depth > depth; ++i)
    {
        if (allEntries_[i].isWorkItem)
        {
            ++total;
            if (allEntries_[i].state == "Completed")
                ++done;
        }
    }
    if (completed)
        *completed = done;
    return total;
}

void TuiApp::rebuildVisibleEntries()
{
    allEntries_ = activeTab_ == 0
        ? engine_.workflowOverview(instanceId_)
        : engine_.workflowTree(instanceId_);
    if (allEntries_.empty())
    {
        visibleEntries_.clear();
        selectedRow_ = 0;
        return;
    }

    // Initialise the root folder expanded; deeper folders start collapsed.
    for (std::size_t i = 0; i < allEntries_.size(); ++i)
    {
        if (hasChildren(i) && expanded_.find(allEntries_[i].id) == expanded_.end())
            expanded_[allEntries_[i].id] = (allEntries_[i].depth == 0);
    }

    Participant *active = activeParticipant();
    std::vector<bool> matches(allEntries_.size(), false);
    for (std::size_t reverse = allEntries_.size(); reverse > 0; --reverse)
    {
        const std::size_t i = reverse - 1;
        const TividyEngine::WorkflowEntry &entry = allEntries_[i];
        if (entry.isWorkItem)
        {
            const bool isOverview = activeTab_ == 0;
            const bool isAll = activeTab_ == 5;
            const bool participantMatch = isOverview || !mineOnly_ || !active ||
                entry.assignee == active->getName() ||
                (entry.assignee == "unassigned" && hasRole(*active, entry.requiredRole));
            const bool stateMatch = isOverview || isAll || stateInTab(activeTab_ - 1, entry.state);
            matches[i] = participantMatch && stateMatch;
        }
        else
        {
            for (std::size_t child = i + 1;
                 child < allEntries_.size() && allEntries_[child].depth > entry.depth;
                 ++child)
            {
                if (matches[child])
                {
                    matches[i] = true;
                    break;
                }
            }
            // Empty composites are still meaningful workflow structure. Show
            // them in All so the user can select the container and add children.
            if (!matches[i] && (activeTab_ == 0 || activeTab_ == 5) && !hasChildren(i))
                matches[i] = true;
        }
    }

    visibleEntries_.clear();
    std::vector<bool> treeVisible(allEntries_.size(), false);
    std::vector<std::size_t> ancestors;
    for (std::size_t i = 0; i < allEntries_.size(); ++i)
    {
        while (!ancestors.empty() && allEntries_[ancestors.back()].depth >= allEntries_[i].depth)
            ancestors.pop_back();

        bool parentAllows = true;
        if (!ancestors.empty())
        {
            const std::size_t parent = ancestors.back();
            const std::map<std::string, bool>::const_iterator expanded =
                expanded_.find(allEntries_[parent].id);
            parentAllows = treeVisible[parent] && expanded != expanded_.end() && expanded->second;
        }
        treeVisible[i] = matches[i] && parentAllows;
        if (treeVisible[i])
            visibleEntries_.push_back(allEntries_[i]);

        if (hasChildren(i))
            ancestors.push_back(i);
    }

    if (visibleEntries_.empty())
        selectedRow_ = 0;
    else if (selectedRow_ >= visibleEntries_.size())
        selectedRow_ = visibleEntries_.size() - 1;
}

void TuiApp::renderTree()
{
    WINDOW *window = treeWindow_.get();
    werase(window);
    const int width = getmaxx(window);
    const int height = getmaxy(window);
    const int idWidth = 9;
    const int ownerWidth = std::max(12, std::min(20, width / 6));
    const int stateWidth = 19;
    const int titleWidth = std::max(16, width - idWidth - ownerWidth - stateWidth - 8);
    const int ownerX = 2 + idWidth + titleWidth + 2;
    const int stateX = ownerX + ownerWidth + 2;

    wattron(window, A_BOLD | A_UNDERLINE);
    mvwprintw(window, 0, 2, "%-*s  %-*s  %-*s  %-*s",
              idWidth, "ID", titleWidth, "TASK / FOLDER",
              ownerWidth, "OWNER", stateWidth, "STATE");
    wattroff(window, A_BOLD | A_UNDERLINE);

    const int visibleRows = height - 1;
    if (selectedRow_ < treeOffset_)
        treeOffset_ = selectedRow_;
    if (selectedRow_ >= treeOffset_ + static_cast<std::size_t>(visibleRows))
        treeOffset_ = selectedRow_ - static_cast<std::size_t>(visibleRows) + 1;
    if (treeOffset_ > visibleEntries_.size())
        treeOffset_ = 0;

    if (visibleEntries_.empty())
    {
        wattron(window, A_DIM);
        safePrint(window, 2, 4, width - 8,
                  mineOnly_ ? "No matching tasks for this participant in this tab. Press f for all tasks."
                            : "No tasks in this tab.");
        wattroff(window, A_DIM);
        return;
    }

    for (int line = 0; line < visibleRows; ++line)
    {
        const std::size_t index = treeOffset_ + static_cast<std::size_t>(line);
        if (index >= visibleEntries_.size())
            break;

        const TividyEngine::WorkflowEntry &entry = visibleEntries_[index];
        const bool selected = index == selectedRow_;
        const bool folder = !entry.isWorkItem;
        if (selected)
        {
            wattron(window, COLOR_PAIR(CP_SELECTED));
            mvwhline(window, line + 1, 0, ' ', width);
        }

        std::ostringstream title;
        if (folder)
        {
            // Count/expand state against the authoritative full snapshot, not
            // just the filtered rows currently displayed in the tree.
            std::size_t fullIndex = 0;
            while (fullIndex < allEntries_.size() && allEntries_[fullIndex].id != entry.id)
                ++fullIndex;
            const std::map<std::string, bool>::const_iterator expanded = expanded_.find(entry.id);
            const bool isExpanded = expanded != expanded_.end() && expanded->second;
            const bool expandable = fullIndex < allEntries_.size() && hasChildren(fullIndex);
            if (expandable)
                title << (isExpanded ? "[-] " : "[+] ");
            else
                title << "[ ] ";
            title << entry.name;
            if (fullIndex < allEntries_.size())
            {
                int completed = 0;
                const int fullTotal = countDescendantItems(fullIndex, &completed);
                title << " (" << completed << "/" << fullTotal << " done)";
            }
        }
        else
            title << "- " << entry.name;

        const int titleIndent = 2 + idWidth + 2;
        const int depthIndent = static_cast<int>(entry.depth) * 2;
        const std::string titleText = title.str();

        if (selected)
            mvwprintw(window, line + 1, 0, ">");
        mvwprintw(window, line + 1, 2, "%-*.*s", idWidth, idWidth, entry.id.c_str());
        if (folder)
            wattron(window, A_BOLD | COLOR_PAIR(selected ? CP_SELECTED : CP_FOLDER));
        mvwprintw(window, line + 1, titleIndent + depthIndent, "%.*s",
                  std::max(1, titleWidth - depthIndent), titleText.c_str());
        if (folder)
            wattroff(window, A_BOLD | COLOR_PAIR(selected ? CP_SELECTED : CP_FOLDER));

        const std::string owner = entry.isWorkItem ? entry.assignee : "<composite>";
        mvwprintw(window, line + 1, ownerX, "%-*.*s", ownerWidth, ownerWidth, owner.c_str());

        const int color = stateColor(entry.state);
        if (!selected)
            wattron(window, COLOR_PAIR(color) | A_BOLD);
        mvwprintw(window, line + 1, stateX, "%-*.*s", stateWidth, stateWidth,
                  displayState(entry.state).c_str());
        if (!selected)
            wattroff(window, COLOR_PAIR(color) | A_BOLD);
        if (selected)
            wattroff(window, COLOR_PAIR(CP_SELECTED));
    }

    if (treeOffset_ > 0)
        mvwprintw(window, 0, width - 4, "^");
    if (treeOffset_ + static_cast<std::size_t>(visibleRows) < visibleEntries_.size())
        mvwprintw(window, 0, width - 3, "v");
}

const TividyEngine::WorkflowEntry *TuiApp::selectedEntry() const
{
    if (visibleEntries_.empty() || selectedRow_ >= visibleEntries_.size())
        return NULL;
    return &visibleEntries_[selectedRow_];
}

void TuiApp::renderDetails()
{
    WINDOW *window = detailWindow_.get();
    werase(window);
    mvwhline(window, 0, 0, ACS_HLINE, getmaxx(window));
    const TividyEngine::WorkflowEntry *entry = selectedEntry();
    const int width = getmaxx(window);
    if (!entry)
    {
        safePrint(window, 1, 2, width - 4, "No task selected.");
        safePrint(window, 5, 2, width - 4, statusMessage_);
        return;
    }

    const bool folder = !entry->isWorkItem;
    wattron(window, A_BOLD);
    std::ostringstream firstLine;
    firstLine << "#" << entry->id << "  " << entry->name;
    if (folder)
    {
        std::size_t fullIndex = 0;
        while (fullIndex < allEntries_.size() && allEntries_[fullIndex].id != entry->id)
            ++fullIndex;
        if (fullIndex < allEntries_.size())
        {
            int completed = 0;
            const int total = countDescendantItems(fullIndex, &completed);
            firstLine << "  [folder: " << completed << "/" << total << " done]";
        }
    }
    safePrint(window, 1, 2, width - 4, firstLine.str());
    wattroff(window, A_BOLD);

    if (folder)
    {
        std::size_t fullIndex = 0;
        while (fullIndex < allEntries_.size() && allEntries_[fullIndex].id != entry->id)
            ++fullIndex;
        const bool expandable = fullIndex < allEntries_.size() && hasChildren(fullIndex);
        safePrint(window, 2, 2, width - 4,
                  expandable ? "Composite node | Enter/Space: expand or collapse this folder"
                             : "Empty composite | no children have been added yet");
        safePrint(window, 3, 2, width - 4,
                  "G: create nested composite | B: add subtask | T: create root task");
        safePrint(window, 5, 2, width - 4, statusMessage_);
    }
    else
    {
        std::ostringstream detail;
        detail << "Owner: " << entry->assignee << " | Required role: " << entry->requiredRole
               << " | State: " << displayState(entry->state);
        safePrint(window, 2, 2, width - 4, detail.str());

        std::ostringstream decorators;
        decorators << "Decorators: ";
        if (entry->decorators.empty())
            decorators << "none";
        else
            for (std::size_t i = 0; i < entry->decorators.size(); ++i)
            {
                if (i) decorators << " | ";
                decorators << entry->decorators[i];
            }
        safePrint(window, 3, 2, width - 4, decorators.str());

        std::ostringstream actions;
        Participant *active = activeParticipant();
        const bool isAssignee = active && entry->assignee == active->getName();
        if (entry->state == "Pending")
        {
            actions << "Assignment locked: waiting for the workflow's sequential gate.";
        }
        else if (entry->state == "Available" || entry->state == "Escalated")
        {
            if (active && hasRole(*active, entry->requiredRole))
                actions << "[A] Assign to active participant  ";
            else
                actions << "Active assignment needs role: " << entry->requiredRole << "  ";
            actions << "[M] Auto assign (" << assignmentStrategyLabel(engine_.assignmentMode(instanceId_)) << ")";
        }
        else if (entry->state == "Assigned")
        {
            if (isAssignee)
                actions << "[S] Start";
            else
                actions << "Assigned to another participant; switch with P";
        }
        else if (entry->state == "InProgress")
        {
            if (isAssignee)
                actions << "[C] Complete  [X] Reject  [E] Edit field";
            else
                actions << "Only the assigned participant can complete, reject or edit this item.";
        }
        else if (entry->state == "AwaitingApproval")
            actions << "[V] Submit through configured approval chain";
        else
            actions << "No state-changing actions available.";
        safePrint(window, 4, 2, width - 4, actions.str());

        if (entry->state == "Pending" && !entry->availabilityBlockerId.empty())
        {
            std::ostringstream blocker;
            blocker << "Waiting for #" << entry->availabilityBlockerId << " "
                    << entry->availabilityBlockerName;
            if (!entry->availabilityBlockerState.empty())
                blocker << " [" << entry->availabilityBlockerState << "]";
            safePrint(window, 5, 2, width - 4, blocker.str());

            safePrint(window, 6, 2, width - 4,
                      "Sequential stage: " + entry->availabilityStageName);
        }
        else if (entry->state == "Pending")
        {
            safePrint(window, 5, 2, width - 4,
                      "No unfinished sibling in a sequential Stage was identified.");
            safePrint(window, 6, 2, width - 4,
                      statusMessage_.empty()
                          ? "The item may need its workflow branch activated."
                          : statusMessage_);
        }
        else
        {
            safePrint(window, 5, 2, width - 4, statusMessage_);
        }
    }

    // Pending tasks use the final two detail rows for a precise blocker and
    // its sequential Stage. Other entries keep the normal transient status.
    if (folder)
        safePrint(window, 6, 2, width - 4, "U: Undo last command");
    else if (entry->state != "Pending")
        safePrint(window, 6, 2, width - 4, "U: Undo last command");
}

void TuiApp::renderLog()
{
    WINDOW *window = logWindow_.get();
    werase(window);
    mvwhline(window, 0, 0, ACS_HLINE, getmaxx(window));
    wattron(window, A_BOLD);
    safePrint(window, 0, 2, getmaxx(window) - 4, "Recent workflow events (Observer)");
    wattroff(window, A_BOLD);

    const int available = getmaxy(window) - 1;
    const std::size_t start = eventLog_.size() > static_cast<std::size_t>(available)
        ? eventLog_.size() - static_cast<std::size_t>(available) : 0;
    int row = 1;
    for (std::size_t i = start; i < eventLog_.size() && row < getmaxy(window); ++i, ++row)
        safePrint(window, row, 2, getmaxx(window) - 4, eventLog_[i]);
    if (eventLog_.empty())
    {
        wattron(window, A_DIM);
        safePrint(window, 1, 2, getmaxx(window) - 4, "Waiting for workflow events...");
        wattroff(window, A_DIM);
    }
}

void TuiApp::renderFooter()
{
    WINDOW *window = footerWindow_.get();
    werase(window);
    wbkgd(window, COLOR_PAIR(CP_FOOTER));
    wattron(window, COLOR_PAIR(CP_FOOTER));

    safePrint(window, 0, 1, getmaxx(window) - 2,
              "Move: arrows/J/K | Tabs: arrows/H/L/Tab");
    safePrint(window, 1, 1, getmaxx(window) - 2,
              "1-6 tabs (1 Overview) | Enter/Space expand | Q quit");
    safePrint(window, 2, 1, getmaxx(window) - 2,
              "P switch | N participant | r add role | R remove role | F filter");
    safePrint(window, 3, 1, getmaxx(window) - 2,
              "T task | G composite | B subtask | [/] strategy | M auto");
    safePrint(window, 4, 1, getmaxx(window) - 2,
              "A assign | S start | C complete | X reject | V approve | E edit | U undo");

    wattroff(window, COLOR_PAIR(CP_FOOTER));
}

void TuiApp::collectEvents()
{
    const std::vector<std::string> fresh = observer_->takePendingEvents();
    for (std::size_t i = 0; i < fresh.size(); ++i)
    {
        eventLog_.push_back(fresh[i]);
        if (eventLog_.size() > 300)
            eventLog_.pop_front();
    }
}

Participant *TuiApp::activeParticipant() const
{
    if (participants_.empty() || activeParticipant_ >= participants_.size())
        return NULL;
    return participants_[activeParticipant_].get();
}

void TuiApp::setMessage(const std::string &message)
{
    statusMessage_ = message;
}

std::string TuiApp::promptLine(const std::string &prompt, int maxLength)
{
    char buffer[512];
    buffer[0] = '\0';
    move(LINES - 1, 0);
    clrtoeol();
    mvprintw(LINES - 1, 1, "%.*s", COLS - 3, prompt.c_str());
    refresh();
    timeout(-1);
    echo();
    curs_set(1);
    getnstr(buffer, std::min(maxLength, static_cast<int>(sizeof(buffer) - 1)));
    noecho();
    curs_set(0);
    timeout(200);
    return std::string(buffer);
}

int TuiApp::promptInteger(const std::string &prompt, int fallback)
{
    const std::string input = promptLine(prompt);
    if (input.empty())
        return fallback;
    char *end = NULL;
    errno = 0;
    const long parsed = std::strtol(input.c_str(), &end, 10);
    if (errno || !end || *end != '\0' || parsed < 0 || parsed > 100000)
        throw std::invalid_argument("Please enter a whole number between 0 and 100000");
    return static_cast<int>(parsed);
}

void TuiApp::switchParticipant()
{
    if (participants_.empty())
    {
        setMessage("No participants registered. Press n to add one.");
        return;
    }
    activeParticipant_ = (activeParticipant_ + 1) % participants_.size();
    selectedRow_ = 0;
    treeOffset_ = 0;
    rebuildVisibleEntries();
    setMessage("Active participant: " + participants_[activeParticipant_]->getName());
}

void TuiApp::changeTab(int direction)
{
    activeTab_ = (activeTab_ + TAB_COUNT + direction) % TAB_COUNT;
    selectedRow_ = 0;
    treeOffset_ = 0;
    rebuildVisibleEntries();
    setMessage(std::string("Showing tab: ") + TAB_NAMES[activeTab_]);
}

void TuiApp::promptAddParticipant()
{
    try
    {
        const std::string name = promptLine("New participant name:");
        if (name.empty())
            return;
        const int authority = promptInteger("Authority level (0+):", 1);
        const std::vector<std::string> roles = splitRoles(promptLine("Roles, comma-separated:"));
        addParticipant(name, authority, roles);
        activeParticipant_ = participants_.size() - 1;
        rebuildVisibleEntries();
        setMessage("Participant added: " + name);
    }
    catch (const std::exception &error)
    {
        setMessage(error.what());
    }
}

void TuiApp::promptAddRole()
{
    Participant *active = activeParticipant();
    if (!active)
    {
        setMessage("Add a participant first (n).");
        return;
    }
    const std::string role = promptLine("Role to add to " + active->getName() + ":");
    if (role.empty())
        return;
    if (active->hasRole(role))
    {
        setMessage("Participant already has role '" + role + "'.");
        return;
    }
    active->addRole(role);
    rebuildVisibleEntries();
    setMessage("Added role '" + role + "'.");
}

void TuiApp::promptRemoveRole()
{
    Participant *active = activeParticipant();
    if (!active)
    {
        setMessage("Add a participant first (n).");
        return;
    }
    const std::string role = promptLine("Role to remove from " + active->getName() + ":");
    if (role.empty())
        return;
    if (!active->removeRole(role))
    {
        setMessage("Participant does not have role '" + role + "'.");
        return;
    }
    rebuildVisibleEntries();
    setMessage("Removed role '" + role + "'.");
}

void TuiApp::promptEditField()
{
    const TividyEngine::WorkflowEntry *entry = selectedEntry();
    Participant *active = activeParticipant();
    if (!entry || !entry->isWorkItem)
    {
        setMessage("Select a work item first.");
        return;
    }
    if (!active || entry->assignee != active->getName())
    {
        setMessage("Only the active participant assigned to this item can edit its fields.");
        return;
    }
    if (entry->state != "InProgress")
    {
        setMessage("Fields can only be edited while the item is InProgress.");
        return;
    }

    const std::string field = promptLine("Field name:");
    if (field.empty())
        return;
    const std::string value = promptLine("Value:");
    try
    {
        engine_.setItemField(instanceId_, entry->id, active->getName(), field, value);
        setMessage("Updated field '" + field + "'.");
        rebuildVisibleEntries();
    }
    catch (const std::exception &error)
    {
        setMessage(error.what());
    }
}

std::string TuiApp::selectedCompositeId(bool requireSelectedComposite) const
{
    const TividyEngine::WorkflowEntry *selected = selectedEntry();
    if (selected && !selected->isWorkItem)
        return selected->id;

    if (requireSelectedComposite)
        throw std::runtime_error("Select a composite folder first (Enter expands it; B adds a subtask).");

    // When a leaf is selected, use its closest composite parent. If nothing is
    // selected (or only the workflow root is present), fall back to the root.
    if (selected)
    {
        std::size_t index = 0;
        while (index < allEntries_.size() && allEntries_[index].id != selected->id)
            ++index;
        if (index < allEntries_.size())
        {
            for (std::size_t i = index; i > 0; --i)
            {
                const TividyEngine::WorkflowEntry &candidate = allEntries_[i - 1];
                if (!candidate.isWorkItem && candidate.depth < selected->depth)
                    return candidate.id;
            }
        }
    }

    if (allEntries_.empty() || allEntries_.front().isWorkItem)
        throw std::runtime_error("The workflow has no composite root to contain new nodes.");
    return allEntries_.front().id;
}

void TuiApp::refreshAfterMutation(const std::string &focusId)
{
    // Re-read the authoritative Facade projection, reset scrolling, and force
    // ncurses to repaint the entire terminal. This avoids stale/overlapping
    // composite rows after assignment or structural changes.
    treeOffset_ = 0;
    rebuildVisibleEntries();
    if (!focusId.empty())
    {
        for (std::size_t i = 0; i < visibleEntries_.size(); ++i)
        {
            if (visibleEntries_[i].id == focusId)
            {
                selectedRow_ = i;
                break;
            }
        }
    }
    collectEvents();
    clearok(stdscr, TRUE);
    erase();
    render();
}

WorkItemDecoratorOptions TuiApp::promptDecoratorOptions()
{
    WorkItemDecoratorOptions options;
    options.validateRequiredFields = isYes(
        promptLine("Apply ValidatedItem? Require all required fields before completion [y/N]:"));

    if (isYes(promptLine("Apply SecuredItem? Restrict assignment/start to permitted roles [y/N]:")))
    {
        options.permittedRoles = splitRoles(
            promptLine("Permitted roles (comma-separated; e.g. analyst, manager):"));
        if (options.permittedRoles.empty())
            throw std::invalid_argument("SecuredItem requires at least one permitted role.");
    }

    options.usePriority = isYes(
        promptLine("Apply PriorityItem? [y/N]:"));
    if (options.usePriority)
    {
        options.priority = promptInteger("Priority (1 = highest, 5 = lowest):", 3);
        if (options.priority < 1 || options.priority > 5)
            throw std::invalid_argument("Priority must be between 1 and 5.");
    }
    return options;
}

void TuiApp::cycleAssignmentStrategy(int direction)
{
    const char *modes[] = {"role", "workload", "rule"};
    const int modeCount = 3;
    const std::string current = engine_.assignmentMode(instanceId_);
    int index = 0;
    for (int i = 0; i < modeCount; ++i)
        if (current == modes[i]) index = i;
    index = (index + modeCount + direction) % modeCount;
    engine_.setAssignmentMode(instanceId_, modes[index]);
    setMessage(std::string("Automatic assignment strategy changed to ") +
               assignmentStrategyLabel(modes[index]) +
               ". Press M on an Available task to use it.");
}

void TuiApp::promptCreateTask()
{
    try
    {
        const std::string name = promptLine("New top-level task name:");
        if (name.empty())
            return;
        const std::string role = promptLine("Required role:");
        if (role.empty())
            throw std::invalid_argument("A required role is needed for the new task.");
        const std::vector<std::string> fields =
            splitRoles(promptLine("Required fields (comma-separated, optional):"));
        const WorkItemDecoratorOptions decorators = promptDecoratorOptions();

        const std::string id = engine_.createTask(instanceId_, name, role, fields, decorators);
        activeTab_ = 5;
        mineOnly_ = false;
        if (!allEntries_.empty())
            expanded_[allEntries_.front().id] = true;
        setMessage("Created task " + id + " - " + name + " in the workflow root.");
        refreshAfterMutation(id);
    }
    catch (const std::exception &error)
    {
        setMessage(error.what());
        refreshAfterMutation();
    }
}

void TuiApp::promptCreateComposite()
{
    try
    {
        const std::string parentId = selectedCompositeId(false);
        const std::string name = promptLine("New composite name:");
        if (name.empty())
            return;
        const std::string typeInput = promptLine("Type: 1 = Stage, 2 = SubProcess:");
        const std::string type = (typeInput == "1" || typeInput.empty()) ? "stage" :
                                 (typeInput == "2" ? "subprocess" : "");
        if (type.empty())
            throw std::invalid_argument("Choose 1 for Stage or 2 for SubProcess.");

        bool parallel = false;
        int sourceVersion = 1;
        if (type == "stage")
        {
            const std::string mode = promptLine("Stage mode: 1 = sequential, 2 = parallel:");
            if (!mode.empty() && mode != "1" && mode != "2")
                throw std::invalid_argument("Choose 1 for sequential or 2 for parallel.");
            parallel = (mode == "2");
        }
        else
        {
            sourceVersion = promptInteger("SubProcess source version (1+):", 1);
            if (sourceVersion < 1)
                throw std::invalid_argument("Source version must be 1 or greater.");
        }

        const std::string id = engine_.createComposite(instanceId_, parentId, name,
                                                        type, parallel, sourceVersion);
        activeTab_ = 5;
        mineOnly_ = false;
        expanded_[parentId] = true;
        expanded_[id] = true;
        setMessage("Created " + type + " " + id + " inside " + parentId + ".");
        refreshAfterMutation(id);
    }
    catch (const std::exception &error)
    {
        setMessage(error.what());
        refreshAfterMutation();
    }
}

void TuiApp::promptCreateSubtask()
{
    try
    {
        const std::string parentId = selectedCompositeId(true);
        const std::string name = promptLine("New subtask name:");
        if (name.empty())
            return;
        const std::string role = promptLine("Required role:");
        if (role.empty())
            throw std::invalid_argument("A required role is needed for the new subtask.");
        const std::vector<std::string> fields =
            splitRoles(promptLine("Required fields (comma-separated, optional):"));
        const WorkItemDecoratorOptions decorators = promptDecoratorOptions();

        const std::string id = engine_.createSubtask(instanceId_, parentId, name, role, fields, decorators);
        activeTab_ = 5;
        mineOnly_ = false;
        expanded_[parentId] = true;
        setMessage("Created subtask " + id + " inside " + parentId + ".");
        refreshAfterMutation(id);
    }
    catch (const std::exception &error)
    {
        setMessage(error.what());
        refreshAfterMutation();
    }
}

void TuiApp::executeSelectedAction(char action)
{
    const TividyEngine::WorkflowEntry *entry = selectedEntry();
    if (!entry || !entry->isWorkItem)
    {
        setMessage("Select a leaf work item first.");
        return;
    }
    const std::string itemId = entry->id;
    if ((action == 'a' || action == 'm') && entry->state == "Pending")
    {
        if (!entry->availabilityBlockerId.empty())
        {
            setMessage("Cannot assign yet: finish " + entry->availabilityBlockerName +
                       " (" + entry->availabilityBlockerId + ") in " +
                       entry->availabilityStageName + ".");
        }
        else
            setMessage("Cannot assign yet: this item has not been activated by its workflow stage.");
        return;
    }

    Participant *active = activeParticipant();
    if (action != 'm' && !active)
    {
        setMessage("Add a participant first (N).");
        return;
    }

    try
    {
        if (action == 'a')
        {
            if (!hasRole(*active, entry->requiredRole))
                throw std::runtime_error("Participant does not have required role '" + entry->requiredRole + "'.");
            engine_.assignItem(instanceId_, itemId, active->getName());
            setMessage("Assigned " + itemId + " to " + active->getName() + ".");
        }
        else if (action == 'm')
        {
            engine_.assignNext(instanceId_, itemId);
            std::string assignedTo = "selected participant";
            const std::vector<TividyEngine::WorkflowEntry> fresh = engine_.workflowTree(instanceId_);
            for (std::size_t i = 0; i < fresh.size(); ++i)
                if (fresh[i].isWorkItem && fresh[i].id == itemId)
                {
                    assignedTo = fresh[i].assignee;
                    break;
                }
            setMessage("Strategy '" + engine_.assignmentMode(instanceId_) +
                       "' assigned " + itemId + " to " + assignedTo + ".");
        }
        else if (action == 's')
        {
            engine_.startItem(instanceId_, itemId, active->getName());
            setMessage("Started " + itemId + ".");
        }
        else if (action == 'c')
        {
            engine_.completeItem(instanceId_, itemId, active->getName());
            setMessage("Completion requested for " + itemId + ". Check the approval tab if required.");
        }
        else if (action == 'x')
        {
            const std::string reason = promptLine("Rejection reason:");
            engine_.rejectItem(instanceId_, itemId, active->getName(),
                               reason.empty() ? "Rejected from TIVIDY TUI" : reason);
            setMessage("Rejected " + itemId + ".");
        }
        else if (action == 'v')
        {
            const std::string decision = engine_.submitApproval(instanceId_, itemId,
                                                                 active->getName(), "complete");
            setMessage("Approval chain: " + decision);
        }
    }
    catch (const std::exception &error)
    {
        setMessage(error.what());
    }
    refreshAfterMutation();
}

void TuiApp::handleInput(int key)
{
    if (key == 'q' || key == 'Q')
    {
        running_ = false;
        return;
    }
    if (key == KEY_RESIZE)
    {
        clear();
        resizeWindows();
        return;
    }
    if (key == KEY_UP || key == 'k')
    {
        if (selectedRow_ > 0)
            --selectedRow_;
        return;
    }
    if (key == KEY_DOWN || key == 'j')
    {
        if (selectedRow_ + 1 < visibleEntries_.size())
            ++selectedRow_;
        return;
    }
    if (key == KEY_LEFT || key == 'h')
    {
        changeTab(-1);
        return;
    }
    if (key == KEY_RIGHT || key == 'l' || key == '\t')
    {
        changeTab(1);
        return;
    }
    if (key == '\n' || key == KEY_ENTER || key == '\r' || key == ' ')
    {
        const TividyEngine::WorkflowEntry *entry = selectedEntry();
        if (!entry)
            return;
        if (!entry->isWorkItem)
        {
            const std::map<std::string, bool>::const_iterator current = expanded_.find(entry->id);
            const bool expand = current == expanded_.end() || !current->second;
            expanded_[entry->id] = expand;
            setMessage(std::string(expand ? "Expanded: " : "Collapsed: ") + entry->name);
            rebuildVisibleEntries();
        }
        else
            setMessage("Selected " + entry->id + " - " + entry->name);
        return;
    }

    switch (key)
    {
    case '1': activeTab_ = 0; selectedRow_ = treeOffset_ = 0; rebuildVisibleEntries(); break;
    case '2': activeTab_ = 1; selectedRow_ = treeOffset_ = 0; rebuildVisibleEntries(); break;
    case '3': activeTab_ = 2; selectedRow_ = treeOffset_ = 0; rebuildVisibleEntries(); break;
    case '4': activeTab_ = 3; selectedRow_ = treeOffset_ = 0; rebuildVisibleEntries(); break;
    case '5': activeTab_ = 4; selectedRow_ = treeOffset_ = 0; rebuildVisibleEntries(); break;
    case '6': activeTab_ = 5; selectedRow_ = treeOffset_ = 0; rebuildVisibleEntries(); break;
    case 'p': case 'P': switchParticipant(); break;
    case 'n': case 'N': promptAddParticipant(); break;
    case 't': case 'T': promptCreateTask(); break;
    case 'g': case 'G': promptCreateComposite(); break;
    case 'b': case 'B': promptCreateSubtask(); break;
    case 'r': promptAddRole(); break;
    case 'R': promptRemoveRole(); break;
    case 'f': case 'F':
        if (activeTab_ == 0)
        {
            setMessage("Overview always shows the complete workflow. Use F on another tab to toggle participant filtering.");
            break;
        }
        mineOnly_ = !mineOnly_;
        selectedRow_ = treeOffset_ = 0;
        rebuildVisibleEntries();
        setMessage(mineOnly_ ? "Showing active participant's tasks." : "Showing all participants' tasks.");
        break;
    case 'a': case 'A': executeSelectedAction('a'); break;
    case 'm': case 'M': executeSelectedAction('m'); break;
    case '[': cycleAssignmentStrategy(-1); break;
    case ']': cycleAssignmentStrategy(1); break;
    case 's': case 'S': executeSelectedAction('s'); break;
    case 'c': case 'C': executeSelectedAction('c'); break;
    case 'x': case 'X': executeSelectedAction('x'); break;
    case 'v': case 'V': executeSelectedAction('v'); break;
    case 'e': case 'E': promptEditField(); break;
    case 'u': case 'U':
        try
        {
            if (engine_.undoLast(instanceId_))
                setMessage("Last command undone.");
            else
                setMessage("There is no command to undo.");
            refreshAfterMutation();
        }
        catch (const std::exception &error)
        {
            setMessage(error.what());
            refreshAfterMutation();
        }
        break;
    default: break;
    }
}
