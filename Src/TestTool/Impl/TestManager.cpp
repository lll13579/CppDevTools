#include <cstdlib>
#include <iostream>
#include <limits>
#include <sstream>
#include "TestTool/Impl/TestAbortException.hpp"
#include "TestTool/Impl/TestCase.hpp"
#include "TestTool/Impl/TestManager.hpp"
#include "TestTool/TestCustomise.hpp"
#include "TestTool/TestFile.hpp"
#include "Util/Assert.hpp"
#include "Util/SystemCout.hpp"

TestManager& TestManager::instance() {
	static TestManager theInstance;
	return theInstance;
}

TestManager::~TestManager() {
	for (std::vector<TestCase*>::iterator it = tests_.begin(); it != tests_.end(); it++) {
		delete *it;
	}
	tests_.clear();
}

void TestManager::addTest(TestCase* testCase) {
	tests_.push_back(testCase);
}

bool TestManager::runTests(const char* projectName, int argc, const char** argv, TestCustomisePtr customise) {
	DirPath projectTestDir = TestFile::getTestDir(projectName);

	// Make sure that the test output directory exists
	TestFile::createTestDir(projectTestDir);

	if (customise) {
		customise->beforeAllTests();
	}

	// Get a list of tests to run if supplied. If empty then run
	// everything. The pair second argument is the line number or -1 to match
	// all lines in file.
	std::vector<std::pair<String, int>> testsToRun;
	for (int i = 1; i < argc; i++) {
		std::string s = argv[i];
		std::string::size_type pos = s.find('(');
		if (pos == std::string::npos) {
			testsToRun.push_back(std::make_pair(String(s), -1));
		}
		else {
			int line = std::atoi(s.substr(pos + 1).c_str());
			testsToRun.push_back(std::make_pair(String(s.substr(0, pos)), line));
			{
				// Check
				std::ostringstream ss;
				ss << testsToRun.back().first.toUtf8() << "(" << testsToRun.back().second << ")";
				if (ss.str() != s) {
					scout << "Invalid command line argument of " << String(s) << sendl;
					testsToRun.pop_back();
				}
			}
		}
	}

	// Run all the tests or just the selected ones if arguments were supplied.
	std::vector<Uint> failedTestIndexes;
	Uint testCount = 0;
	for (currentTestIndex_ = 0; currentTestIndex_ < tests_.size(); currentTestIndex_++) {
		TestCase* testCase = tests_[currentTestIndex_];
		if (!testsToRun.empty()) {
			bool match = false;
			for (auto it = testsToRun.begin(); it != testsToRun.end(); it++) {
				if ((it->second == -1) || (testCase->getTestLineNumber() == it->second)) {
					if (testCase->getTestFilename().caselessEquals(it->first)) {
						match = true;
						break;
					}
				}
			}
			if (!match) {
				continue;
			}
		}
		if (customise) {
			customise->beforeTestCase();
		}
		if (!testCase->run()) {
			failedTestIndexes.push_back(currentTestIndex_);
		}
		if (customise) {
			customise->afterTestCase();
		}
		testCount++;
	}

	if (customise) {
		customise->afterAllTests();
	}

	// Display a summary of all the errors
	scout << "===== A total of " << testCount << " tests have been run =====" << sendl;
	if (!failedTestIndexes.empty()) {
		if (failedTestIndexes.size() == 1) {
			scout << "The following test has failed:" << sendl;
		}
		else {
			scout << "The following " << failedTestIndexes.size() << " tests have failed:" << sendl;
		}
		for (std::vector<Uint>::const_iterator it = failedTestIndexes.begin(); it != failedTestIndexes.end(); it++) {
			TestCase* tc = tests_[*it];
			scout 
				<< tc->getTestFilename() << "(" << tc->getTestLineNumber() << "): "
				<< tc->getFailureCount() << " error(s)"
				<< sendl;
		}
	}
	else {
		scout << "All tests completed successfully" << sendl;
	}

	if (argc > 1) {
		scout << "Warning: not all test cases were run due to command line argument(s)" << sendl;
	}

	return failedTestIndexes.empty();
}

TestCase* TestManager::getTestCase() {
	if (currentTestIndex_ >= tests_.size()) {
		// Assertion error but we cannot use assert
		scout << "TEST ERROR: assertion error in TestManager::getTestCase" << sendl;
		throw TestAbortException();
	}
	return tests_[currentTestIndex_];
}

TestManager::TestManager() : 
	tests_(), 
	currentTestIndex_(std::numeric_limits<Uint>::max())
{
}
