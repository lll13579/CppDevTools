#include <iostream>
#include "TestTool/Impl/TestAbortException.hpp"
#include "Util/SystemCout.hpp"

template <typename X, typename Y>
void TestCase::testFailure(const char* file, int line, const char* xExpr, const char* yExpr, X x, Y y, bool isFatal) {
	scout	<< "TEST ERROR: At "
			<< file << "(" << line << ") " 
			<< "condition (" << xExpr << " == " << yExpr << ") failed"
			<< " with values (";
	testValueOutput(x);
	scout	<< " == ";
	testValueOutput(y);
	scout	<< ")"
			<< sendl;
	incrementFailureCount();
	if (isFatal) {
		throw TestAbortException();
	}
}

template<typename X>
void TestCase::testValueOutput(X x) {
	scout << x;
}

void TestCase::testValueOutput(std::string x) {
	scout << x.c_str();
}

void TestCase::incrementFailureCount() {
	// Set a breakpoint here to catch call failures as they occur.
	++failureCount_;
}

