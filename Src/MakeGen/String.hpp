#pragma once
#include <sstream>
#include <string>

// Simple class which allows a string argument to be derived from a stream.
// Usage:
//		void myfunc(const std::string& s);
//		...
//		myfunc(String() << "Example " << x << " code" << std::endl);

class String {
public:
	String() : ss_() { }
	template <typename T>
	String(const T& x) : ss_() { ss_ << x; }
	String(const char* s) : ss_() { ss_ << s; }

	template <typename T> String& operator<<(const T& x) {
		ss_ << x;
		return *this;
	}

	operator std::string() const {
		return ss_.str();
	}

private:
	std::ostringstream ss_;
};

inline std::ostream& operator<<(std::ostream& os, const String& s) {
	os << (std::string)s;
	return os;
}
