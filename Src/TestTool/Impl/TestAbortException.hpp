#pragma once
#include <exception>

// Thrown if a test is aborted.
class TestAbortException : public std::exception {
public:
	TestAbortException() { }
	const char* what() const throw() { return "Test abort exception"; }
};
