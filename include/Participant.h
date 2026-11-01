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
        std::vector<const Task*> assignments_;
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
         * @brief Removes the first matching role from this participant.
         * @param role Role to remove.
         * @return true if the role was found and removed; otherwise, false.
         */
        bool removeRole(const std::string &role);
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
        void assign(const Task* task);
        /**
         * @brief Removes one matching task assignment and updates the workload.
         * @param task Task assignment to remove.
         * @return true if an assignment was found and removed; otherwise, false.
         */
        bool unassign(const Task* task);
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
         * @brief Returns the participant's roles.
         * @return A constant reference to the participant's role list.
         */
        const std::vector<std::string>& getRoles() const;
        /**
         * @brief Performs the increment workload operation.
         */
        void incrementWorkload();

};

#endif 