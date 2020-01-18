#pragma once
#include <memory>
#include "TestTool/TestEvent.hpp"
#include "Util/Def.hpp"

// Main test header file. Include this file in any file with test cases.
//
// Typical test usage is:
//
//		AUTO_TEST_CASE {
//			...
//			// Optional
//			Macro call to expect some event (project dependent)
//			EXPECT_EXCEPTION(...);
//
//			// Tests
//			CHECK(...);
//			CHECK_EQUAL(...);
//			REQUIRE(...);
//			REQUIRE_EQUAL(...);
//			...
//		}
//
// The AUTO_TEST_CASE line of the above (assuming it was on line 123 of file "myfile") expands to:
//
//		static void testAtLine123();
//		static TestUtil::TestRegistrar testRegistrationAtLine123("myfile", 123, testAtLine123);
//		static void testAtLine123() {
//

// Macro to notify the test case that it should be aborted by an
// exception which is identical to "ex". So for example call:
// EXPECT_EXCEPTION(FileNotFoundException(filePath));
#define EXPECT_EXCEPTION(ex) do { TestUtil::expectException((ex).what()); } while (false)

// Macro to notify the test case that it should be expecting nothing.
// So all the above EXPECT... events must already have occurred.
#define EXPECT_NOTHING do { TestUtil::expectNothing(); } while (false)

// Macro to start a test case.
#define DELAYED_PASTE1(x,y) DELAYED_PASTE2(x,y)
#define DELAYED_PASTE2(x,y) DELAYED_PASTE3(x,y)
#define DELAYED_PASTE3(x,y) x ## y
#define AUTO_TEST_CASE \
	static void DELAYED_PASTE1(testAtLine, __LINE__)(); \
	static TestUtil::TestRegistrar DELAYED_PASTE1(testRegistrationAtLine, __LINE__) (__FILE__, __LINE__, DELAYED_PASTE1(testAtLine, __LINE__)); \
	static void DELAYED_PASTE1(testAtLine, __LINE__)()

// Macros to check that a boolean value is true. If false then an error is logged.
// If a CHECK fails then the test case continues. If a REQUIRE fails then the test
// case is immediately aborted.
#define CHECK(x)   do { if (!(x)) TestUtil::testFailure(__FILE__, __LINE__, #x, false); } while (false)
#define REQUIRE(x) do { if (!(x)) TestUtil::testFailure(__FILE__, __LINE__, #x, true ); } while (false)

// Similar macros which check for equality and output the values which have failed.
#define CHECK_EQUAL(x, y)   do { if (!((x) == (y))) TestUtil::testFailure(__FILE__, __LINE__, #x, #y, (x), (y), false); } while (false)
#define REQUIRE_EQUAL(x, y) do { if (!((x) == (y))) TestUtil::testFailure(__FILE__, __LINE__, #x, #y, (x), (y), true ); } while (false)

DPTR(TestCustomise)

namespace TestUtil {

	// The function type for the test function.
	typedef void (*TestFunc)();

	// A simple registrar class which constructs and registers a test.
	class TestRegistrar {
	public:
		// file: The source file containing the test
		// line: The source line the test starts at
		// testFunc: The test function to run
		TestRegistrar(const char* file, int line, TestFunc testFunc);
	};

	// Notify the current test case to expect an event. These event notifications
	// are queued by the test case and must occur in order.
	void expectEvent(TestEventPtr event);

	// Notify the current test case to expect an exception which reports the
	// given "what" string. This may be called at most once per test.
	void expectException(const String& what);

	// Notify that no events or exceptions are being expected. So if there
	// are any it is a test failure and these expected events or exceptions
	// are cleared from the test case.
	void expectNothing();

	// An actual event which has occurred during the execution of the test
	// case. This is checked against the next expected event.
	void event(TestEventPtr actualEvent);

	// Notify the current test case that there has been an assertion
	// error with the given hash. The failure is fatal for the test case.
	void testAssertionFailure(std::uint32_t fileLineHash);

	// Notify the current test case that one of its tests has failed.
	// file: The source file containing the test
	// line: The source line in this file which has failed
	// condition: The text of the condition which has failed.
	// isFatal: True if this failure is fatal for the test case.
	void testFailure(const char* file, int line, const char* condition, bool isFatal);

	// Notify the current test case that one of its tests has failed
	// because two values are unequal.
	// file: The source file containing the test
	// line: The source line in this file which has failed
	// xExpr: The text of the x expression.
	// yExpr: The text of the y expression.
	// x: The x value
	// y: The y value
	// isFatal: True if this failure is fatal for the test case.
	template <typename X, typename Y>
	void testFailure(const char* file, int line, const char* xExpr, const char* yExpr, X x, Y y, bool isFatal);

	// Get the number of failures for the current test case.
	Uint getTestFailureCount();

	// Run all registered tests and return true on success.
	//
	// projectName is the name of the sub-directory which holds the test
	// project e.g. "UtilTest" and it is also the project name. argc and argv supply
	// the command line arguments from the test executable. If omitted then all
	// tests are run. Otherwise each argument is a test file path relative to and
	// including the Src directory with optionally a test line number in brackets.
	// If the line number is omitted all tests in that file are run.
	//
	// "customise" set the actions to perform at different points
	// during the testing. Setting null (the default) disables all
	// actions.
	bool runTests(const char* projectName, int argc, const char** argv, TestCustomisePtr customise = TestCustomisePtr());
}

#include "TestTool/Impl/TestUtil.inl.hpp"
