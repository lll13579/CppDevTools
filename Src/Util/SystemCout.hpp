#pragma once
#include <iostream>
#include "Util/Def.hpp"
#include "Util/OutputStream.hpp"
#include "Util/OutputStreamWithIndent.hpp"

DPTR(CharOutputConverter);

// This file supports "scout" which is a streaming output object
// which is similar to std::cout but works consistently with Unicode.
// If the standard output is directed to the console window then
// the output uses the OEM code page in Windows. If redirected
// it uses the ANSI code page.

// Characters passed to scout (e.g. scout << 'a') must be ASCII.
// Character string literals (e.g. scout << "hello") must be UTF8
// (or 7 bit ASCII which is subset of this). The end of line is
// output with scout << sendl.

// An alternative named "icout" is the same but supports indenting
// and is for debugging only.

class SystemCout : public OutputStream {
public:
	SystemCout();
	~SystemCout();
	void streamChar(Char ch);

private:
#if BUILD(WINDOWS)
	// Determine if the std::cout output is going to the console
	// window or has been redirected somewhere else. If there are
	// any internal errors then these return true.
	bool isConsoleOutput();
#endif

private:
	CharOutputConverterPtr converter_;
};

// Global object
extern SystemCout scout;
extern OutputStreamWithIndent icout;
