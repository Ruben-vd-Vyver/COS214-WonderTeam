#include "doctest.h"
#include "test_helpers.h"
#include "ApprovalRequest.h"
#include "AutoApprovalHandler.h"
#include "AuthorityHandler.h"
#include "FinalRejectHandler.h"

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

}
