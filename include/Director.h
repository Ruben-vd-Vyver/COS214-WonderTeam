#ifndef DIRECTOR_H
#define DIRECTOR_H
class WorkflowBuilder;
class Director
{
public:
    virtual ~Director() {}
    virtual bool construct(WorkflowBuilder &b) = 0;
};
#endif // DIRECTOR_H