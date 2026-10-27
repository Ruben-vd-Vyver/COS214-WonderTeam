#include "../include/Participant.h"
#include <sstream>

int Participant::ParticipantCount_ = 0;

Participant::Participant(std::string name, int authorityLevel, std::vector<std::string> roles)
 : name_(name), authorityLevel_(authorityLevel), roles_(roles) {
    ParticipantCount_++;
    if (name == "")
        name_ = "Participant" + ParticipantCount_; 
}

void Participant::addRole(std::string role){
    roles_.push_back(role);
}

bool Participant::hasRole(std::string role) const{
    if (std::find(roles_.begin(), roles_.end(), role) != roles_.end()) {
        return true;
    }
    return false;
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