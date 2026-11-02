#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "AutoApprovalHandler.h"
#include "Participant.h"
#include "AuthorityHandler.h"
#include "FinalRejectHandler.h"
#include "TividyEngine.h"
#include "TuiApp.h"
#include "TuiObserver.h"
#include "WorkflowDirector.h"
#include "WorkflowObserver.h"

int main(int argc, char *argv[])
{
    // The first positional argument is the workflow script to load.
    // Example: ./main test.txt
    if (argc > 2)
    {
        std::cerr << "Usage: " << argv[0] << " [workflow-file]" << std::endl;
        return 2;
    }

    try
    {
        // These strategy/handler objects must outlive the engine definitions and instances.
        AutoApprovalHandler autoApproval(1);
        AuthorityHandler authorityApproval;
        FinalRejectHandler finalReject;
        autoApproval.setNext(&authorityApproval)->setNext(&finalReject);
        // Keep the observer alive for at least as long as the WorkflowInstance subscription.
        std::shared_ptr<TuiObserver> observer(new TuiObserver());

        // Small, generic fallback used only if the bundled example file is unavailable.
        std::istringstream fallbackDemo(
            "workflow | TIVIDY Example | 1\n"
            "task | Task1 | role1\n"
            "approve | complete | review\n"
            "stage | Stage1 | sequential\n"
            "task | Task2 | role1\n"
            "task | Task3 | role2\n"
            "end\n"
            "stage | Stage2 | parallel\n"
            "task | Task4 | role1\n"
            "stage | SubStage1 | sequential\n"
            "task | Task5 | role2\n"
            "task | Task6 | role1\n"
            "end\n"
            "task | Task7 | role2\n"
            "end\n"
            "done\n");
        std::ifstream workflowFile;
        std::istream *workflowInput = &fallbackDemo;
        if (argc > 1)
        {
            workflowFile.open(argv[1]);
            if (!workflowFile)
                throw std::runtime_error(std::string("Cannot open workflow file: ") + argv[1]);
            workflowInput = &workflowFile;
        }
        else
        {
#ifdef TIVIDY_SOURCE_DIR
            const std::string bundledDemo = std::string(TIVIDY_SOURCE_DIR) + "/examples/demo.workflow";
#else
            const std::string bundledDemo = "examples/demo.workflow";
#endif
            workflowFile.open(bundledDemo.c_str());
            if (!workflowFile)
            {
                workflowFile.clear();
                workflowFile.open("examples/demo.workflow");
            }
            if (workflowFile)
                workflowInput = &workflowFile;
        }

        WorkflowDirector director(*workflowInput);
        director.registerApprovalChain("review", &autoApproval);

        // Declare participant ownership before the engine so these shared_ptrs
        // remain alive until after TividyEngine (which stores raw pointers) is destroyed.
        std::vector<std::shared_ptr<Participant> > participantOwners;
        TividyEngine engine;
        const std::string definitionId = engine.deploy(director, "TIVIDY-Demo", 1);
        const std::vector<WorkflowObserver *> observers(1, observer.get());
        const std::string instanceId = engine.startInstance(definitionId, observers);
        engine.setAssignmentMode(instanceId, "role");

        TuiApp app(engine, instanceId, observer, participantOwners);
        // Generic demo participants whose roles match the example workflow.
        app.addParticipant("Participant1", 1, std::vector<std::string>(1, "role1"));
        app.addParticipant("Participant2", 2, std::vector<std::string>(1, "role2"));
        std::vector<std::string> bothRoles;
        bothRoles.push_back("role1");
        bothRoles.push_back("role2");
        app.addParticipant("Participant3", 3, bothRoles);
        return app.run();
    }
    catch (const std::exception &error)
    {
        std::cerr << "TIVIDY startup failed: " << error.what() << std::endl;
        return 1;
    }
}
