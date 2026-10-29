// Bassically a concrete version, can do a dynamic one scenarioDirector
#include "WorkflowDirector.h"
#include "WorkflowBuilder.h"

WorkflowDirector::WorkflowDirector(ApprovalHandler *signOffChain, ApprovalHandler *certChain) : signOffChain_(signOffChain), certChain_(certChain) {}

void WorkflowDirector::construct(WorkflowBuilder &b)
{
    b.reset("Game Content Update", 1);

    b.beginStage("Feature Approval", false);
    b.addTask("Review feature request", "producer");
    if (signOffChain_)
        b.requireApproval("complete", signOffChain_);
    b.endStage();

    b.beginStage("Production", true);
    b.addTask("Create art", "artist");
    b.addTask("Write code", "programmer");
    b.addTask("Produce audio", "sound designer");
    b.addTask("Write narrative", "writer");
    b.endStage();

    b.beginStage("Integration", false);
    b.addTask("Integrate content", "programmer");
    b.addTask("Run build", "build engineer");
    b.endStage();

    b.beginStage("Quality Assurance", false);
    b.addTask("Test build", "qa tester");
    b.endStage();

    b.beginStage("Sign-off", false);
    b.addTask("Creative director sign-off", "creative director");
    if (signOffChain_)
        b.requireApproval("complete", signOffChain_);
    b.endStage();

    b.beginStage("Platform Certification", true);
    b.addTask("Steam certification", "release engineer");
    if (certChain_)
        b.requireApproval("complete", certChain_);
    b.addTask("PlayStation certification", "release engineer");
    if (certChain_)
        b.requireApproval("complete", certChain_);
    b.addTask("Nintendo certification", "release engineer");
    if (certChain_)
        b.requireApproval("complete", certChain_);
    b.endStage();

    b.beginStage("Release", false);
    b.addTask("Release to storefront", "release manager");
    b.endStage();
}
