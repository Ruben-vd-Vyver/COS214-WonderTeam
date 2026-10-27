#include "Participant.h"
#include <sstream>

int Participant::ParticipantCount_ = 0;

// Initialiser list follows the member declaration order (name_, roles_,authorityLevel_) to avoid -Wreorder warnings.
Participant::Participant(std::string name, int authorityLevel, std::vector<std::string> roles)
 : name_(std::move(name)), roles_(std::move(roles)), authorityLevel_(authorityLevel) {
    ParticipantCount_++;
    if (name_.empty())
        name_ = "Participant" + std::to_string(ParticipantCount_);
}

void Participant::addRole(std::string role){
    roles_.push_back(role);
}

bool Participant::hasRole(std::string role) const{
    return std::find(roles_.begin(), roles_.end(), role) != roles_.end();
}

void Participant::assign(Task* task){
    if (task){
        assignments_.push_back(task);
        workload_++;
    }
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
