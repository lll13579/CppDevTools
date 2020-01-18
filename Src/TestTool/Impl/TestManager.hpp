#pragma once
#include <memory>
#include <vector>

class TestCase;
DPTR(TestCustomise)

// Singleton class which manages all tests.
// It collects a list of all tests and then runs them.
class TestManager {
public:
	// Get the singleton instance of the test manager
	static TestManager& instance();

	// Destructor - deletes all the tests
	~TestManager();

	// Add a test case to the test manager. The test manager
	// takes ownership of the test case.
	void addTest(TestCase* testCase);

	// See TestUtil.hpp
	bool runTests(const char* projectName, int argc, const char** argv, TestCustomisePtr customise);

	// Get the current test case
	TestCase* getTestCase();

private:
	TestManager();

private:
	std::vector<TestCase*> tests_;		// The tests
	Uint currentTestIndex_;				// Index into tests_ for current test when running runTests().
};
