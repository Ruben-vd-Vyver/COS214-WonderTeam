#include "Participant.h"
#include <sstream>

int Participant::ParticipantCount_ = 0;

// Initialiser list follows the member declaration order (name_, roles_,authorityLevel_) 
Participant::Participant(std::string name, int authorityLevel, std::vector<std::string> roles)
 : name_(std::move(name)), roles_(std::move(roles)), authorityLevel_(authorityLevel) {
    ParticipantCount_++;
    if (name_.empty())
        name_ = "Participant" + std::to_string(ParticipantCount_);
}

void Participant::addRole(std::string role){
    roles_.push_back(role);
}

bool Participant::removeRole(const std::string &role){
    std::vector<std::string>::iterator it = std::find(roles_.begin(), roles_.end(), role);
    if (it == roles_.end())
        return false;
    roles_.erase(it);
    return true;
}

bool Participant::hasRole(std::string role) const{
    return std::find(roles_.begin(), roles_.end(), role) != roles_.end();
}

void Participant::assign(const Task* task){
    if (task){
        assignments_.push_back(task);
        workload_++;
    }
}

bool Participant::unassign(const Task* task){
    if (!task)
        return false;
    std::vector<const Task*>::iterator it = std::find(assignments_.begin(), assignments_.end(), task);
    if (it == assignments_.end())
        return false;
    assignments_.erase(it);
    if (workload_ > 0)
        workload_--;
    return true;
}
        
int Participant::getWorkload() const{
    return workload_;
}
        
int Participant::getAuthority() const{
    return authorityLevel_;
}

std::string Participant::describe(){
    std::stringstream description;
    description << name_ << " | Authority: " << authorityLevel_ << " | Roles: ";
    if (roles_.empty()) {
        description << "No roles";
    } else {
        for (std::size_t i = 0; i < roles_.size(); ++i) {
            if (i > 0) description << ", ";
            description << roles_[i];
        }
    }

    return description.str();
}

void Participant::incrementWorkload() {
    workload_++;
}

std::string Participant::getName() const {
    return name_;
}

const std::vector<std::string>& Participant::getRoles() const {
    return roles_;
}
