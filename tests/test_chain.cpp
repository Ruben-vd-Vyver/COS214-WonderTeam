#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "test_helpers.h"
#include "ApprovalRequest.h"
#include "AutoApprovalHandler.h"
#include "AuthorityHandler.h"
#include "FinalRejectHandler.h"
#include "BasicWorkItem.h"
#include "Task.h"
#include "AwaitingApprovalState.h"
#include "RejectedState.h"
#include "Participant.h"
#include "PendingState.h"

using namespace testutil;

TEST_SUITE("Chain of Responsibility") {

TEST_CASE("a request is passed along until a handler decides it") {
	AutoApprovalHandler autoH(1);
	AuthorityHandler authority;
	FinalRejectHandler finalReject;
	autoH.setNext(&authority)->setNext(&finalReject);

	Participant junior("Junior", 2, std::vector<std::string>());
	Participant senior("Senior", 5, std::vector<std::string>());

	SUBCASE("low level: decided by the first handler") {
		ApprovalRequest req(nullptr, &junior, 1);
		autoH.handle(req);
		CHECK(req.isDecided());
		CHECK(req.isApproved());
		CHECK(req.getReason().find("AutoApproval") != std::string::npos);
	}

	SUBCASE("high level, enough authority: passed on to AuthorityHandler") {
		ApprovalRequest req(nullptr, &senior, 4);
		autoH.handle(req);
		CHECK(req.isApproved());
		CHECK(req.getReason().find("Authority") != std::string::npos);
	}

	SUBCASE("high level, not enough authority: falls through to FinalRejectHandler") {
		ApprovalRequest req(nullptr, &junior, 4);
		autoH.handle(req);
		CHECK(req.isDecided());
		CHECK_FALSE(req.isApproved());
		CHECK(req.getReason().find("FinalReject") != std::string::npos);
	}
}
//Edge case: Chain with a missing final fallback (unhandled requests)
    TEST_CASE("unhandled requests remain undecided if no handler catches them") {
        AutoApprovalHandler autoH(0); // Only level 0 auto-approves
        AuthorityHandler authority;
        autoH.setNext(&authority); // No FinalRejectHandler at the end

        Participant junior("Junior", 1, roles("nurse"));
        ApprovalRequest req(nullptr, &junior, 5); //Requires level 5, junior has 1, autoH max is 0

        autoH.handle(req);
        
        // Neither handler should be able to process this request
        CHECK_FALSE(req.isDecided());
        CHECK_FALSE(req.isApproved());
    }

    //Integration test: Chain connected to Task and evaluated via AwaitingApprovalState
    TEST_CASE("AwaitingApprovalState correctly invokes the Task's approval chain on item approval") {
        //Build the chain: AutoApproval (level 1) -> AuthorityHandler -> FinalRejectHandler
        AutoApprovalHandler autoH(1);
        AuthorityHandler authority;
        FinalRejectHandler finalReject;
        autoH.setNext(&authority)->setNext(&finalReject);

        std::shared_ptr<Task> task = std::make_shared<Task>("Surgery Signoff", "doctor");
        task->requireApproval("review", &autoH, 3); //Requires level 3 authority

        BasicWorkItem item("item-surgery", task);
   
        item.advanceTo(new PendingState(), "review");
        CHECK(item.getStateName() == "AwaitingApproval");

        SUBCASE("Approval succeeds when requester has high enough authority") {
            Participant seniorDoctor("Chief Surgeon", 5, roles("doctor"));
            item.setAssignee(&seniorDoctor);

            //Calling approve() should evaluate the chain, pass AuthorityHandler, and transition out of AwaitingApproval
            item.approve();
            CHECK(item.getStateName() != "AwaitingApproval");
            CHECK(item.getStateName() != "Rejected");
        }

        SUBCASE("Approval is rejected when requester has insufficient authority") {
            Participant juniorNurse("Nurse Joy", 1, roles("nurse"));
            item.setAssignee(&juniorNurse);

            //Calling approve() should fail the chain, hitting FinalRejectHandler, moving to RejectedState
            item.approve();
            CHECK(item.getStateName() == "Rejected");
        }
    }
}
