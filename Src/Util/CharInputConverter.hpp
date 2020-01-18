#pragma once
#include <memory>
#include <string>
#include "Util/Char.hpp"
#include "Util/CharEncoding.hpp"
#include "Util/CharInputConverterErrorHandler.hpp"
#include "Util/String.hpp"

DPTR(CharInputConverter)
DPTR(CharInputConverterErrorHandler)

// An input converter which converts a sequence of source bytes into a set 
// of internal characters or strings.
class CharInputConverter {
public:
	// The maximum size in bytes for an external character across all possible encodings.
	// This is a very conservative value.
	static const Uint MAX_INPUT_CHAR_BYTES = 16;

	// Create a character converter which converts from the supplied source encoding
	// (which must be valid) into the internal encoding. The error handler reports
	// conversion errors. If null then errors are not reported. After an encoding error,
	// errorChar is output to replace the faulty input character. Set this to Char::eof()
	// if the error needs to be detected externally since this character can never occur
	// through valid input.
	static CharInputConverterPtr create(
		CharEncoding srcEncoding, 
		CharInputConverterErrorHandlerPtr errorHandler, 
		Char errorChar);

	// Destructor
	virtual ~CharInputConverter();

	// Get the source encoding
	CharEncoding getSrcEncoding() const { return srcEncoding_; }

	// Converts one character.
	//
	// src points to the source buffer and srcLen is the number of source bytes
	// in this buffer which should be at least MAX_INPUT_CHAR_BYTES unless there
	// are fewer bytes available (e.g. near EOF). srcLen must be non-zero.
	//
	// The first return value is the number of source characters which were consumed.
	// The second return value is the converted character.
	//
	// If there is an encoding error (including if srcLen is so small that there is
	// only a partial character in the buffer), then an error message is output if there
	// is a location provider, and the erroneous bytes are stripped from the input
	// (up to a maximum of the smaller of MAX_INPUT_CHAR_BYTES and srcLen). The first
	// return value is the number of source characters to be stripped and the second
	// return value is the error character.
	virtual std::pair<Uint, Char> convertChar(const char* src, Uint srcLen) = 0;

	// Convenience method which converts the whole of the source buffer src of length
	// srcLen or src.size(). Errors are handled as for convertChar() and the error
	// character is inserted into the output string as appropriate. The entire source
	// buffer must be used up: any partial character at the end is treated as an error.
	// The converted output is appended on to the supplied string.
	void convertAppend(const char* src, Uint srcLen, String& dst);
	void convertAppend(const std::string& src, String& dst);

	// Convenience method as above which returns the destination string.
	String convertString(const char* src, Uint srcLen);
	String convertString(const std::string& src);

protected:
	// Constructor. Same arguments as create().
	CharInputConverter(
		CharEncoding srcEncoding, 
		CharInputConverterErrorHandlerPtr errorHandler, 
		Char errorChar);

	// Report an error with the given bytes and pass it on to the error handler
	void reportError(const char* src, Uint srcLen) const;

protected:
	CharEncoding srcEncoding_;							// The source encoding
	CharInputConverterErrorHandlerPtr errorHandler_;	// A handler for errors
	Char errorChar_;									// The error character
};
