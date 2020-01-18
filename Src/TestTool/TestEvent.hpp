#pragma once
#include <memory>
#include "Util/String.hpp"

// An event which has occurred during testing which needs to be
// be monitored and checked. A test lists the expected events
// in order and then the test manager checks that these occur
// as expected.

DPTR(TestEvent)

class TestEvent {
public:
	virtual ~TestEvent() { }

	// Compare this event with another. Returns true if
	// the same.
	virtual bool isIdenticalTo(TestEventPtr other) const = 0;

	// Convert the event into a string which can be displayed
	virtual String toString() const = 0;
};
