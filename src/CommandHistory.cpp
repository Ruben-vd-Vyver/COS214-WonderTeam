#include "../include/CommandHistory.h"  

CommandHistory::~CommandHistory(){
    for (WorkItemCommand* work : history_)
        delete work;
    for (WorkItemCommand* work : future_)
        delete work;
}
 
bool CommandHistory::run(WorkItemCommand* work){
    if (!work)
        return false;
    bool check = work->execute();
    if (check){
        history_.push_back(work);

        for (WorkItemCommand* work : future_) {
            delete work; 
        }

        future_.clear();
    } else {
        std::cout << "Error running command\n";
        return false;
    }
    if (hist_limit < history_.size()){
        delete history_.front();
        history_.erase(history_.begin());
    }
    return true;
}
    
bool CommandHistory::undo(){
    if (!history_.size())
        return false;
    WorkItemCommand* work = history_.back();
    bool check = work->undo();

    if (check){
        history_.pop_back();
        future_.push_back(work);
    } else {
        std::cout << "Error reverting command\n";
        return check;
    }
    if (fut_limit < future_.size()){
        delete future_.front();
        future_.erase(future_.begin());
    }
    return check;
};

bool CommandHistory::redo(){
    if (!future_.size())
        return false;
    WorkItemCommand* work = future_.back();
    bool check = work->execute();
    if (check){
        history_.push_back(work);
        future_.pop_back();
    } else {
        std::cout << "Error running command\n";
        return check;
    }
    if (hist_limit < history_.size()){
        delete history_.front();
        history_.erase(history_.begin());
    }
    return check;
};

std::vector<std::string> CommandHistory::log(){
    std::vector<std::string> log;
    for (WorkItemCommand* work : history_) {
        log.push_back(work->describe());
    }
    return log;
};