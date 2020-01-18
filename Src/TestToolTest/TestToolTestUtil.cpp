#include <sstream>
#include "TestTool/TestEvent.hpp"
#include "TestTool/TestUtil.hpp"
#include "Util/Def.hpp"

class LocalTestEvent : public TestEvent {
public:
	static TestEventPtr create(Uint u) {
		return std::make_shared<LocalTestEvent>(u);
	}

	~LocalTestEvent() {
	}

	bool isIdenticalTo(TestEventPtr other) const {
		LocalTestEvent* p = dynamic_cast<LocalTestEvent*>(other.get());
		return p && (u_ == p->u_);
	}

	String toString() const {
		std::ostringstream ss;
		ss << u_;
		return String("The value is ") + String(ss.str());
	}

private:
	LocalTestEvent(Uint u) : u_(u) { }
	ALLOW_MAKE_SHARED(LocalTestEvent);
	Uint u_;
};

class LocalTestException : public std::exception {
public:
	LocalTestException() { }
	const char* what() const throw() { return "Local test exception"; }
};

AUTO_TEST_CASE {
	TestUtil::expectEvent(LocalTestEvent::create(22));
	TestUtil::event(LocalTestEvent::create(22));
	REQUIRE(true);
}

AUTO_TEST_CASE {
	TestUtil::expectEvent(LocalTestEvent::create(57));
	TestUtil::expectEvent(LocalTestEvent::create(22));
	TestUtil::expectEvent(LocalTestEvent::create(33));
	TestUtil::event(LocalTestEvent::create(57));
	TestUtil::event(LocalTestEvent::create(22));
	TestUtil::event(LocalTestEvent::create(33));
	CHECK(TestUtil::getTestFailureCount() == 0);
	EXPECT_NOTHING;
}

AUTO_TEST_CASE {
	CHECK(true);
	EXPECT_EXCEPTION(LocalTestException());
	REQUIRE(true);
	throw LocalTestException();
	REQUIRE(false);
}

AUTO_TEST_CASE {
	CHECK(true);
	CHECK(5 == 5);
	CHECK_EQUAL(5, 5);
	// The following will fail and so are commented out
	// CHECK(false);
	// CHECK(1 == 2);
	// CHECK_EQUAL(1, 2);
}

AUTO_TEST_CASE {
	REQUIRE(true);
	REQUIRE(5 == 5);
	REQUIRE_EQUAL(5, 5);
	// The following will fail and so are commented out
	// REQUIRE(false);
	// REQUIRE(1 == 2);
	// REQUIRE_EQUAL(1, 2);
}
