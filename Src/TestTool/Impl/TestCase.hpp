#pragma once
#include <vector>
#include "TestTool/TestUtil.hpp"

// A single test case.
// It runs a particular test in isolation.
class TestCase {
public:
	// The test case constructor.
	// file: The source file containing the test as returned by the __FILE__ C++ macro
	// line: The source line the test starts at as returned by the __LINE__ C++ macro
	// testFunc: The test function to run the test
	TestCase(const char* file, int line, TestUtil::TestFunc testFunc);

	// Run the test and return true on success.
	bool run();

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

	// An assertion error with the given hash. The failure is fatal for the
	// test case.
	void testAssertionFailure(std::uint32_t fileLineHash);

	// A test failure
	// file: The source file containing the test
	// line: The source line in this file which has failed
	// condition: The text of the condition which has failed.
	// isFatal: True if this failure is fatal for the test case.
	void testFailure(const char* file, int line, const char* condition, bool isFatal);

	// A test failure
	// file: The source file containing the test
	// line: The source line in this file which has failed
	// xExpr: The text of the x expression.
	// yExpr: The text of the y expression.
	// x: The x value
	// y: The y value
	// isFatal: True if this failure is fatal for the test case.
	template <typename X, typename Y>
	void testFailure(const char* file, int line, const char* xExpr, const char* yExpr, X x, Y y, bool isFatal);

	// Get the name of the source file which contains the test.
	// This is the value supplied to the constructor but with everything up to and
	// including the /src/ directory stripped off and the remaining path separated
	// with the "/" character and all in lowercase. 
	String getTestFilename() const { return testFilename_; }

	// Get the source line number which starts the test
	int getTestLineNumber() const { return testLineNumber_; }

	// Get the number of failures in the test
	Uint getFailureCount() const { return failureCount_; }

private:
	template<typename X> void testValueOutput(X x);
	inline void testValueOutput(std::string x);
	inline void incrementFailureCount();

private:
	String testFilename_;			// The source file for the test (stripped of path up to /src/
									// and all in lower case).
	int testLineNumber_;			// The source line number for the start of the test
	TestUtil::TestFunc testFunc_;	// The test function
	String whatException_;			// The what() for an exception which is expected for the 
									// test or empty if no exception is expected.
	std::vector<TestEventPtr> expectedEvents_;	// The expected events
	Uint failureCount_;				// The number of test failures
};

#include "TestTool/Impl/TestCase.inl.hpp"
