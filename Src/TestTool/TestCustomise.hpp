#pragma once
#include <memory>
#include "Util/Def.hpp"

// Actions to be performed at different points in the testing.
// Derive from this class to set up custom actions which are then
// supplied to the test manager.

class TestCustomise {
public:
	virtual ~TestCustomise() { }

	// Action to perform once before running all test cases
	virtual void beforeAllTests() = 0;

	// Action to perform once after running all test cases
	virtual void afterAllTests() = 0;

	// Action to perform before running each test case
	virtual void beforeTestCase() = 0;

	// Action to perform after running each test case
	virtual void afterTestCase() = 0;
};
DPTR(TestCustomise)


