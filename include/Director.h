#ifndef DIRECTOR_H
#define DIRECTOR_H
/**
 * @brief Represents the workflow builder abstraction in the TIVIDY workflow system.
 */
class WorkflowBuilder;
/**
 * @brief Represents the director abstraction in the TIVIDY workflow system.
 */
class Director
{
public:
    /**
     * @brief Destroys this object and releases owned resources.
     */
    virtual ~Director() {}
    /**
     * @brief Performs the construct operation.
     * @param b Value or object supplied by the caller.
     * @return Result produced by the operation.
     */
    virtual bool construct(WorkflowBuilder &b) = 0;
};
#endif // DIRECTOR_H