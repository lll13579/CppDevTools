#pragma once
#include <string>
#include "Util/Def.hpp"

class String;

// The character encoding for something external like a file.
class CharEncoding {
public:
	// The different types of encoding
	enum Ordinal {
		INVALID,		// An invalid encoding
#if BUILD(WINDOWS)
		ANSI,			// The Windows ANSI code page which is used by default for
						// files edited in Notepad, Wordpad and Visual Studio.
						// Code page 1252 for English Windows by default.
		OEM,			// The Windows OEM code page which is used by the console
						// window. Code page 850 for English Windows by default.
#endif
		UTF8,			// UTF-8
		UTF16BE,		// UTF-16 big endian
		UTF16LE,		// UTF-16 little endian
		UTF32BE,		// UTF-32 big endian
		UTF32LE,		// UTF-32 little endian
		DEFAULT			// The default encoding				
#if BUILD(WINDOWS)
		= ANSI			// ANSI eg. English_United Kingdom.1252 under Windows.
#elif BUILD(LINUX)
		= TODO			// LINUX TODO
#else
#error
#endif
	};

	// Constructor from a name. The names are the same as listed above for
	// Ordinal but in lowercase. An unrecognised name sets the encoding to
	// INVALID.
	explicit CharEncoding(const String& name);

	// Constructor from an ordinal
	CharEncoding(Ordinal ordinal = INVALID);

	// Returns true if the encoding is valid (i.e. not invalid).
	bool isValid() const;

	// Get the encoding name. The encoding must be valid.
	// The names are the same as listed above for Ordinal but in lowercase.
	String getName() const;

	// Get the encoding ordinal
	Ordinal getOrdinal() const { return ordinal_; }

	// Comparison
	bool operator==(const CharEncoding& other) const { return ordinal_ == other.ordinal_; }
	bool operator!=(const CharEncoding& other) const { return ordinal_ != other.ordinal_; }

private:
	Ordinal ordinal_;
};
