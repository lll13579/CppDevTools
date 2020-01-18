#pragma once
#include "Util/Def.hpp"

// A single character as used for internal purposes. The character must either be a
// valid character from the allowed UTF range or the special EOF character. The
// internal representation of the character is currently UTF32 but this may change in future. 
class Char {
public:
	// Default constructor. The value is the same as EOF.
	Char();

	// Constructor from ASCII value. The argument must be 7 bit ASCII.
	Char(char ch);

	// Returns true if an uppercase letter from 'A' to 'Z'.
	bool isUpper() const;

	// Returns true if a lowercase letter from 'a' to 'z'.
	bool isLower() const;

	// Returns true if an uppercase or lowercase letter
	// from 'A' to 'Z' or from 'a' to 'z'.
	bool isLetter() const;

	// Returns true if a digit from '0' to '9'.
	bool isDecimalDigit() const;

	// Returns true if a binary digit of '0' or '1'.
	bool isBinaryDigit() const;

	// Returns true if a digit from '0' to '7'.
	bool isOctalDigit() const;

	// Returns true if a hexadecimal digit from '0' to '9', 'A' to 'F' or 'a' to 'f'.
	bool isHexDigit() const;

	// Returns true if a whitespace character i.e. one of the 5 characters in: " \t\v\f\n"
	bool isWhitespace() const;

	// Returns true if a character from the C++ basic source character set (see CPP2.2[1]).
	bool isBasicSource() const;

	// Returns true if a character which is allowed in a Pp identifier.
	bool isPpIdentifier() const;

	// Returns true if the special character we have picked to represent end of file.
	bool isEof() const;

	// Returns true if the character is from the ASCII range 0x00 to 0x7f
	bool isAscii() const;

	// Returns the decimal value of characters from '0' to '9'.
	// The character must be a decimal digit.
	Uint decimalValue() const;

	// Returns the binary value of characters of '0' and '1'.
	// The character must be a binary digit.
	Uint binaryValue() const;

	// Returns the octal value of characters from '0' to '7'.
	// The character must be a octal digit.
	Uint octalValue() const;

	// Returns the hexadecimal value of characters from '0' to '9', 'a' to 'f' or 'A' to 'F'.
	// The character must be a hexadecimal digit.
	Uint hexValue() const;

	// Convert the character to uppercase if it is a lowercase letter from 'a' to 'z'
	// otherwise return it unchanged.
	Char toUpperCopy() const;

	// Convert the character to lowercase if it is uppercase letter from 'A' to 'Z'
	// otherwise return it unchanged.
	Char toLowerCopy() const;

	// The EOF character
	static Char eof();

	// Get the character which represents the given decimal digit.
	// The value must be in the range 0 to 9.
	static Char decimalDigit(Uint value);

	// Get the character which represents the given octal digit
	// The value must be in the range 0 to 7.
	static Char octalDigit(Uint value);

	// Get the character which represents the given hex digit.
	// The value must be in the range 0 to 15.
	// Hex letters are either upper or lower case depending on the argument.
	static Char hexDigit(Uint value, bool upperNotLower);

	// Create a character from 1 to 4 UTF8 bytes. len is set to the number of bytes
	// in the UTF8 character (even if the character is invalid). Returns the character
	// on success or EOF on failure. If less than 4 bytes are available utf8 should
	// be null terminated.
	static Char fromUtf8(const char* utf8, Uint& len);

	// Create a character from 1 to 2 UTF16 words. len is set to the number of words
	// in the UTF16 character (even if the character is invalid). Returns the character
	// on success or EOF on failure. If less than 2 words are available utf16 should
	// be null terminated.
	static Char fromUtf16(const char16_t* utf16, Uint& len);

	// Create a character from a UTF32 word. Returns the character on success or
	// EOF on failure. 
	static Char fromUtf32(char32_t utf32);

	// Converts the character to 1 to 4 UTF8 bytes. Returns the number of UTF8 bytes converted.
	// utf8 must have room for 4 bytes.
	Uint toUtf8(char* utf8);

	// Converts the character to 1 to 2 UTF16 words. Returns the number of UTF16 words converted.
	// utf16 must have room for 2 words.
	Uint toUtf16(char16_t* utf16);

	// Converts the character to 1 UTF32 word. EOF is converted to (char32_t)-1.
	char32_t toUtf32();

	// Comparison. Tests whether this character and another are identical.
	bool operator==(Char other) const;
	bool operator!=(Char other) const;

	// Comparison against char. The char must be a 7 bit ASCII value.
	bool operator==(char other) const;
	bool operator!=(char other) const;

private:
	// Construct from the supplied value which must be valid UTF32 or EOF.
	Char(Uint32 ch);

private:
	friend class String;

	// The character value in UTF32 or (Uint32)-1 for EOF.
	Uint32 ch_;
};

// Allow streaming to an OutputStream object.
class OutputStream;
OutputStream& operator<<(OutputStream& os, Char ch);
