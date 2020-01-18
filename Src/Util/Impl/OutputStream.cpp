#include <iostream>
#include <string>
#include "Util/Assert.hpp"
#include "Util/OutputStream.hpp"
#include "Util/String.hpp"

OutputStream& operator<<(OutputStream& os, bool x) {
	FAIL;
}

OutputStream& operator<<(OutputStream& os, char x) {
	os << Char(x);
	return os;
}

OutputStream& operator<<(OutputStream& os, const char* x) {
	os << String(x);
	return os;
}

OutputStream& operator<<(OutputStream& os, int x) {
	std::string s = std::to_string(x);
	os << String(s);
	return os;
}

OutputStream& operator<<(OutputStream& os, long x) {
	std::string s = std::to_string(x);
	os << String(s);
	return os;
}

OutputStream& operator<<(OutputStream& os, long long x) {
	std::string s = std::to_string(x);
	os << String(s);
	return os;
}

OutputStream& operator<<(OutputStream& os, unsigned int x) {
	std::string s = std::to_string(x);
	os << String(s);
	return os;
}

OutputStream& operator<<(OutputStream& os, unsigned long x) {
	std::string s = std::to_string(x);
	os << String(s);
	return os;
}

OutputStream& operator<<(OutputStream& os, unsigned long long x) {
	std::string s = std::to_string(x);
	os << String(s);
	return os;
}

