#ifndef TIVIDY_TUI_APP_H
#define TIVIDY_TUI_APP_H

#include <deque>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "TividyEngine.h"
#include "TuiObserver.h"
#include "TuiWindow.h"

class Participant;

// ncurses presentation/controller. All workflow reads and mutations go through
// TividyEngine; this class never manipulates WorkItem/WorkflowNode objects.
class TuiApp
{
public:
    TuiApp(TividyEngine &engine, const std::string &instanceId,
           const std::shared_ptr<TuiObserver> &observer,
           std::vector<std::shared_ptr<Participant> > &participantOwners);
    ~TuiApp();

    TuiApp(const TuiApp &) = delete;
    TuiApp &operator=(const TuiApp &) = delete;

    // Participant shared_ptrs must outlive TividyEngine, which stores raw pointers.
    void addParticipant(const std::string &name, int authority,
                        const std::vector<std::string> &roles);
    int run();

private:
    static const int TAB_COUNT = 6;

    TividyEngine &engine_;
    std::string instanceId_;
    std::shared_ptr<TuiObserver> observer_;
    std::vector<std::shared_ptr<Participant> > &participants_;
    std::size_t activeParticipant_;

    bool running_;
    bool cursesActive_;
    bool mineOnly_;
    int activeTab_;
    std::string statusMessage_;
    std::vector<TividyEngine::WorkflowEntry> allEntries_;
    std::vector<TividyEngine::WorkflowEntry> visibleEntries_;
    std::map<std::string, bool> expanded_;
    std::size_t selectedRow_;
    std::size_t treeOffset_;
    std::deque<std::string> eventLog_;

    // The reference-style layout: header, tabs, full-width tree, action panel,
    // Observer log and compact key footer.
    TuiWindow topWindow_;
    TuiWindow tabsWindow_;
    TuiWindow treeWindow_;
    TuiWindow detailWindow_;
    TuiWindow logWindow_;
    TuiWindow footerWindow_;

    void initNcurses();
    void endNcurses();
    void resizeWindows();
    void render();
    void renderTop();
    void renderTabs();
    void renderTree();
    void renderDetails();
    void renderLog();
    void renderFooter();
    void collectEvents();
    void rebuildVisibleEntries();
    void handleInput(int key);
    void executeSelectedAction(char action);
    void switchParticipant();
    void changeTab(int direction);
    void promptAddParticipant();
    void promptAddRole();
    void promptRemoveRole();
    void promptEditField();
    void promptCreateTask();
    void promptCreateComposite();
    void promptCreateSubtask();
    WorkItemDecoratorOptions promptDecoratorOptions();
    void cycleAssignmentStrategy(int direction);
    void refreshAfterMutation(const std::string &focusId = std::string());
    std::string selectedCompositeId(bool requireSelectedComposite) const;
    std::string promptLine(const std::string &prompt, int maxLength = 180);
    int promptInteger(const std::string &prompt, int fallback);

    bool hasChildren(std::size_t index) const;
    int countDescendantItems(std::size_t index, int *completed = NULL) const;
    int countTabItems(int tab) const;
    const TividyEngine::WorkflowEntry *selectedEntry() const;
    Participant *activeParticipant() const;
    void setMessage(const std::string &message);
};

#endif // TIVIDY_TUI_APP_H
