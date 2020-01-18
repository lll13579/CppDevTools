#include <iostream>
#include "TestTool/Impl/TestAbortException.hpp"
#include "TestTool/Impl/TestCase.hpp"
#include "Util/Assert.hpp"
#include "Util/File.hpp"
#include "Util/SystemCout.hpp"

TestCase::TestCase(const char* file, int line, TestUtil::TestFunc testFunc) :
	testFilename_(),
	testLineNumber_(line),
	testFunc_(testFunc),
	whatException_(),
	expectedEvents_(),
	failureCount_(0)
{
	// Convert to lowercase. Windows does this anyway so we need to do it across all
	// platforms to allow correct testing for particular filenames.
	String fullFile = String(file).toLowerCopy();

	// Strip the supplied filename to remove everything before the Src or Build directory.
	// (Build is needed for automatically generated source files).
	String separator = FileSystem::getPathSeparator();
	String sourcePathFragment = separator + "src" + separator;
	StringIterPair pos = fullFile.findFirst(sourcePathFragment);
	if (!pos.atEnd()) {
		testFilename_ = "src/";
	}
	else {
		String buildPathFragment = separator + "build" + separator;
		pos = fullFile.findFirst(buildPathFragment);
		ASSERT(!pos.atEnd());
		testFilename_ = "build/";
	}

	for (StringIter it = pos.second(); !it.atEnd(); it++) {
		if (*it == '\\') {
			// Replace \ with /
			testFilename_ += Char('/');
		}
		else {
			testFilename_ += (*it);
		}
	}
}

// Run the test and return true on success.
bool TestCase::run() {
	scout << "===== " << testFilename_ << "(" << testLineNumber_ << ")" << " =====" << sendl;
	try {
		(*testFunc_)();

		// The test has completed so all events and exceptions should have occurred.
		expectNothing();
	}
	catch (TestAbortException&) {
		// We are expecting this so nothing to do. The test has been aborted
		// with a fatal error so we do not check the remaining events or
		// exception.
	}
	catch (std::exception& ex) {
		String what = ex.what();
		if (!whatException_.empty() &&
			(whatException_ == what))
		{
			// We have received the exception we were expecting
			whatException_.clear();

			// The test has completed so all events should have occurred.
			expectNothing();
		}
		else {
			scout << "TEST ERROR: Unexpected exception \"" << what << "\"" << sendl;
			incrementFailureCount();
		}
	}
	catch (...) {
		scout << "TEST ERROR: Unexpected unknown exception" << sendl;
		incrementFailureCount();
	}

	// Free resources
	expectedEvents_.clear();

	if (failureCount_ > 0) {
		scout << ">->-> " << failureCount_ << " error(s)" << sendl; 
	}

	return (failureCount_ == 0);
}

void TestCase::expectEvent(TestEventPtr event) {
	expectedEvents_.push_back(event);
}

void TestCase::expectException(const String& what) {
	ASSERT(whatException_.empty());
	whatException_ = what;
}

void TestCase::expectNothing() {
	if (!expectedEvents_.empty()) {
		for (Uint i = 0; i < expectedEvents_.size(); i++) {
			TestEventPtr event = expectedEvents_[i];
			scout << "TEST ERROR: The following expected event did not occur: " << event->toString() << sendl;
		}
		incrementFailureCount();
		expectedEvents_.clear();
	}
	if (!whatException_.empty()) {
		scout << "TEST ERROR: The following expected exception was not thrown: " << whatException_ << sendl;
		incrementFailureCount();
		whatException_.clear();
	}
}

void TestCase::event(TestEventPtr actualEvent) {
	if (expectedEvents_.empty()) {
		scout << "TEST ERROR: Unexpected report message: " << actualEvent->toString() << sendl;
		incrementFailureCount();
	}
	else {
		TestEventPtr expectedEvent = expectedEvents_.front();
		expectedEvents_.erase(expectedEvents_.begin());
		
		if (!actualEvent->isIdenticalTo(expectedEvent)) {
			scout << "TEST ERROR: Incorrect event" << sendl;
			scout << "TEST ERROR: Expected event: " << expectedEvent->toString() << sendl;
			scout << "TEST ERROR: Actual event:   " << actualEvent->toString() << sendl;
			incrementFailureCount();
		}
	}
}

void TestCase::testAssertionFailure(std::uint32_t fileLineHash) {
	scout	<< "TEST ERROR: Assertion error with hash "
			<< (fileLineHash / 1000000)
			<< "-"
			<< (fileLineHash % 1000000)
			<< sendl;
	incrementFailureCount();
	throw TestAbortException();
}

void TestCase::testFailure(const char* file, int line, const char* condition, bool isFatal) {
	scout	<< "TEST ERROR: At "
			<< file << "(" << line << ") " 
			<< "condition (" << condition << ") failed"
			<< sendl;
	incrementFailureCount();
	if (isFatal) {
		throw TestAbortException();
	}
}

