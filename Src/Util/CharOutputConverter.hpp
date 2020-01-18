#pragma once
#include <memory>
#include <string>
#include "Util/Char.hpp"
#include "Util/CharEncoding.hpp"
#include "Util/CharInputConverterErrorHandler.hpp"
#include "Util/String.hpp"

DPTR(CharOutputConverter)

// An output converter which converts a set of internal characters or strings
// into a sequence of destination bytes. Note that more than one source character
// may be be mapped to one output character. For example for an output converter
// to the Windows UK locale, the letters such as A or C with diacritical marks
// are all mapped to the appropriate uppercase or lowercase ASCII A or C.
class CharOutputConverter {
public:
	// The maximum size in bytes for an external character across all possible encodings.
	// This is a very conservative value.
	static const Uint MAX_OUTPUT_CHAR_BYTES = 16;

	// Create a character converter which converts from the internal UTF32 encoding 
	// to the supplied destination encoding (which must be valid).
	static CharOutputConverterPtr create(CharEncoding dstEncoding);

	// Destructor
	virtual ~CharOutputConverter();

	// Get the destination encoding
	CharEncoding getDstEncoding() const { return dstEncoding_; }

	// Converts one character.
	//
	// ch is the input character (which must not be EOF).
	// 
	// dst points to the destination buffer which must be at least MAX_OUTPUT_CHAR_BYTES
	// in size.
	//
	// The return value is the number of destination bytes written. If 0 then there
	// has been an encoding error: the character ch cannot be represented in the
	// output encoding.
	virtual Uint convertChar(Char ch, char* dst) = 0;

	// Convenience method which converts the whole of the source string and appends
	// it on to the supplied std::string. Returns true on success or false if there
	// was one or more encoding errors. Characters which cannot be encoded are skipped.
	bool convertAppend(const String& src, std::string& dst);

	// Convenience method as above which returns the destination string. If there are
	// any encoding errors then the returned string is empty.
	std::string convertString(const String& src);

protected:
	// Constructor
	CharOutputConverter(CharEncoding dstEncoding);

private:
	CharEncoding dstEncoding_;		// The destination encoding
};
