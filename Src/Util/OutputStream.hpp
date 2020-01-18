#pragma once
#include "Util/Char.hpp"

// A interface which allows Char objects to be streamed.
class OutputStream {
public:
	OutputStream() { }
	virtual ~OutputStream() { }
	
	// Override this to perform the streaming on characters
	virtual void streamChar(Char ch) = 0;
};

// Streaming boolean values is not allowed (assertion error)
OutputStream& operator<<(OutputStream& os, bool x);

// The character must be ASCII
OutputStream& operator<<(OutputStream& os, char x);

// The character string must be UTF8
OutputStream& operator<<(OutputStream& os, const char* x);

// Any C++ integer type other than char.
OutputStream& operator<<(OutputStream& os, int x);
OutputStream& operator<<(OutputStream& os, long x);
OutputStream& operator<<(OutputStream& os, long long x);
OutputStream& operator<<(OutputStream& os, unsigned int x);
OutputStream& operator<<(OutputStream& os, unsigned long x);
OutputStream& operator<<(OutputStream& os, unsigned long long x);

// A named enumeration is a unique object so the type alone is sufficient
// to work out what to do. sendl is the end of line manipulator for the
// output stream.
enum SendlType { sendl };

inline OutputStream& operator<<(OutputStream& os, SendlType) {
	os.streamChar(Char('\n'));
	return os;
}

