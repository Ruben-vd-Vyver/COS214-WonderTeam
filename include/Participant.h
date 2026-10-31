<<<<<<< HEAD
#ifndef PARTICIPANT_H
#define PARTICIPANT_H

#include <string>
#include <vector>
#include <algorithm>
#include "Task.h"

/*
    --- Participant Class ---
    Participants act as "Employees" or "Users" in the workload. 
    They can be assigned work, and execute commands
    and (depending on authority) are involved in the approval process. 
*/

class Participant {
    private:
        std::string name_;
        std::vector<std::string> roles_;
        int authorityLevel_;
        std::vector<Task*> assignments_;
        int workload_ = 0; // How much task/work is currently assigned to this participant
        static int ParticipantCount_;
    public:
        Participant(std::string name, int authorityLevel, std::vector<std::string> roles);
        ~Participant() = default;
        void addRole(std::string role);
        bool hasRole(std::string role) const;
        void assign(Task* task);
        int getWorkload() const;
        int getAuthority() const;
        std::string describe();

};

#endif //PARTICIPANT_H
=======
#ifndef PARTICIPANT_H
#define PARTICIPANT_H

#include <string>
#include <vector>
#include <algorithm>

class Task; //forward declaration (Task.h also refers to Participant)

    // Participants act as "Employees" or "Users" in the workload. 
    // They can be assigned work, and execute commands and (depending on authority) are involved in the approval process. 


class Participant {
    private:
        std::string name_;
        std::vector<std::string> roles_;
        int authorityLevel_;
        std::vector<Task*> assignments_;
        int workload_ = 0; //how much task/work is currently assigned to this participant
        static int ParticipantCount_;
    public:
        Participant(std::string name, int authorityLevel, std::vector<std::string> roles);
        ~Participant() = default;
        void addRole(std::string role);
        bool hasRole(std::string role) const;
        void assign(Task* task);
        int getWorkload() const;
        int getAuthority() const;
        std::string describe();
        std::string getName() const;
        void incrementWorkload();

};

#endif //PARTICIPANT_H
>>>>>>> 1f0f34d0501dd8ca2ab898b1338ffbcd48b3b34c
