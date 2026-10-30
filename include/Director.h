#ifndef DIRECTOR_H
#define DIRECTOR_H
class WorkflowBuilder;
class Director
{
public:
    virtual ~Director() {}
    virtual void construct(WorkflowBuilder &b) = 0;
};
#endif // DIRECTOR_H