#include <iostream>
#include <string>
#include "Util/Assert.hpp"
#include "Util/CharOutputConverter.hpp"
#include "Util/SystemCout.hpp"
#include "Util/Windows.hpp"

SystemCout::SystemCout() : 
	OutputStream(),
	converter_()
{
#if BUILD(WINDOWS)
	converter_ = CharOutputConverter::create(
		isConsoleOutput() ? CharEncoding::OEM : CharEncoding::ANSI);
#else
	// Linux etc use UTF8.
	converter_ = CharOutputConverter::create(CharEncoding::UTF8);
#endif
}

SystemCout::~SystemCout() {
}

void SystemCout::streamChar(Char ch) {
	char buf[CharOutputConverter::MAX_OUTPUT_CHAR_BYTES];
	Uint size = converter_->convertChar(ch, buf);

	if (size == 0) {
		// There has been an encoding error. Just output '?'.
		std::cout << '?';
	}
	else {
		for (Uint i = 0; i < size; ++i) {
			std::cout << buf[i];
		}
	}
}

#if BUILD(WINDOWS)
bool SystemCout::isConsoleOutput() {
	// Get the stdout handle
	HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if ((stdHandle == INVALID_HANDLE_VALUE) || (stdHandle == NULL)) {
		// Some error - assume console
		return true;
	}

	// Test the file type. If not a character device it cannot be the console.
	DWORD fileType = GetFileType(stdHandle);
	if (fileType != FILE_TYPE_CHAR) {
		return false;
	}

	// Look up the console mode. The return status will be zero if it
	// is not the console. consoleMode is not used.
	DWORD consoleMode;
	BOOL status = GetConsoleMode(stdHandle, &consoleMode);
	return (status != 0);
}
#endif

// Global objects
SystemCout scout;
OutputStreamWithIndent icout(scout);
