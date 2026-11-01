#ifndef PARTICIPANT_H
#define PARTICIPANT_H

#include <string>
#include <vector>
#include <algorithm>

/**
 * @brief Represents the task abstraction in the TIVIDY workflow system.
 */
class Task; 

    // Participants act as "Employees" or "Users" in the workload. 
    /**
     * @brief Performs the and operation.
     * @param authority Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
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
        /**
         * @brief Constructs an object of this type.
         * @param name Value or object supplied by the caller.
         * @param authorityLevel Value or object supplied by the caller.
         * @param roles Value or object supplied by the caller.
         */
        Participant(std::string name, int authorityLevel, std::vector<std::string> roles);
        /**
         * @brief Destroys this object and releases owned resources.
         */
        ~Participant() = default;
        /**
         * @brief Adds the role.
         * @param role Value or object supplied by the caller.
         */
        void addRole(std::string role);
        /**
         * @brief Reports whether the requested condition is satisfied.
         * @param role Value or object supplied by the caller.
         * @return Result produced by the operation.
         */
        bool hasRole(std::string role) const;
        /**
         * @brief Performs the assign operation.
         * @param task Value or object supplied by the caller.
         */
        void assign(Task* task);
        /**
         * @brief Returns the workload.
         * @return Result produced by the operation.
         */
        int getWorkload() const;
        /**
         * @brief Returns the authority.
         * @return Result produced by the operation.
         */
        int getAuthority() const;
        /**
         * @brief Performs the describe operation.
         * @return Result produced by the operation.
         */
        std::string describe();
        /**
         * @brief Returns the name.
         * @return Result produced by the operation.
         */
        std::string getName() const;
        /**
         * @brief Performs the increment workload operation.
         */
        void incrementWorkload();

};

#endif 
