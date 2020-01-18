#pragma once
#include <string>
#include "Util/Char.hpp"
#include "Util/Def.hpp"
#include "Util/OutputStream.hpp"

class String;
class StringIterPair;

// This class can iterate through the Char characters within a String.
// The StringIter becomes invalid and must not be used if the referenced
// String is modified.
class StringIter {
public:
	// Copy constructor
	StringIter(const StringIter& other);

	// Assignment
	StringIter& operator=(const StringIter& other);
	
	// Get the character at the current position. Returns Char::eof() at
	// the end of the string.
	Char operator*() const { return ch_; }

	// Get a pointer to the character at the current position. Must not
	// be at the end of the string. This allows this class to work in
	// situations such as:
	// StringIter it = ...;
	// Char ch = it->toUpperCopy();
	const Char* operator->() const;

	// Comparison. The other string position must be for the same string.
	// Returns a comparison of the two string positions.
	bool operator==(const StringIter& other) const;
	bool operator!=(const StringIter& other) const;
	bool operator<(const StringIter& other) const;
	bool operator<=(const StringIter& other) const;
	bool operator>(const StringIter& other) const;
	bool operator>=(const StringIter& other) const;

	// Advance one character position. Has no effect if already at the end
	// of the string.
	StringIter operator++();
	StringIter operator++(int);

	// Go back one character position. Not allowed if already at the beginning
	// of the string.
	StringIter operator--();
	StringIter operator--(int);

	// True if positioned at the beginning of the string.
	bool atBegin() const;

	// True if positioned at the end of the string
	bool atEnd() const;

	// Finds the next occurrence of str in the current string at or after the
	// current position. If found returns the string positions of the beginning
	// and end of the found string. If not found both return positions are set
	// to the end of the string. The current string position is unchanged.
	StringIterPair findNext(const String& str) const;

	// Finds the next occurrence of ch in the current string at or after the
	// current position. If found returns the string position of the found
	// character. If not found returns the position at the end of the string.
	// The current string position is unchanged.
	StringIter findNext(Char ch) const;
		
	// Returns the sub-string before the current position
	String substrBefore() const;

	// Returns the sub-string at the current position and beyond
	String substrAfter() const;

private:
	friend class String;

	// Constructor
	StringIter(const String& s, Uint pos);

	// Initialise the member variables following a change of position.
	void init();

private:
	const String* s_;	// String being iterated over
	Uint pos_;			// Position within s_.str_ NOT Unicode character position within s_
						// (or s_.str_.size() at EOF)
	Char ch_;			// The character at this position (or EOF at end)
	Uint len_;			// The length of the current UTF8 character (or 0 at EOF)
};

// Represents a pair of string iterators in the same string.
class StringIterPair {
public:
	// Constructor. Both first__ and second__ must be for the same string and the
	// position of first__ must be before or at second__.
	StringIterPair(const StringIter& first__, const StringIter& second__);

	// Returns true if both positions are at the end of the string (typically
	// indicating a "not found" condition).
	bool atEnd() { return first_.atEnd(); }

	// Return the first position
	const StringIter& first() { return first_; }

	// Return the second position
	const StringIter& second() { return second_; }

private:
	StringIter first_;	// The first string position
	StringIter second_;	// The second string position
};

// A platform string is a string which holds all Unicode characters in a way which
// is appropriate for the platform. For Windows compilers this is a std::wstring
// which is a string of 16 bit UTF16 wchar_t. For other compilers this is just std::string
// encoded using UTF8. This type can be passed as for example a filename argument for
// std::ifstream and still represent all Unicode values.
#if BUILD(MSV)
typedef std::wstring PlatformString;
#else
typedef std::string PlatformString;
#endif

// A string class which is similar to std::string but encapsulates the behaviour
// required to work with the StringIter and Char classes.
//
// A String object stores its string as Unicode. Currently the internal implementation
// is as a std::string of UTF8 characters but that could change later - for example to
// a std::vector of Char.
//
// The implication of this is that the String class does not know where its characters
// are without iterating through the underlying std::string. So the String class does
// not provide a size() function or [] indexing.
//
// Any char* and std::string arguments must be encoded as UTF8. All such inputs are
// validated and there is an assertion error if they are not valid Unicode. So it is
// up to callers to ensure that these arguments are valid. 
//
// A String is derived from OutputStream so that the << operator can be used for
// appending.

class String : public OutputStream {
public:
	// Constructor. An empty string.
	String() : str_() { }

	// Copy constructor.
	String(const String& s) : str_(s.str_) { }

	// Construct from a null terminated UTF8 string (which must not contain
	// any null characters) and must be valid UTF8.
	String(const char* s) : str_(s) { validate(str_); }
	
	// Construct from a UTF8 string which must be valid UTF8.
	explicit String(const std::string& s) : str_(s) { validate(str_); }

	// Construct from a single character which must not be EOF.
	String(Char ch) : str_() { operator+=(ch); }

	// Construct from a position range. The string starts at "from" and runs
	// up to but not including "to".
	String(const StringIter& from, const StringIter& to);

	// Assignment
	String& operator=(const String& s) { str_ = s.str_; return *this; }

	// OutputStream virtual method
	void streamChar(Char ch) { operator+=(ch); }

	// Iterators to the beginning and end of the string
	StringIter begin() const { return StringIter(*this, 0); }
	StringIter end() const { return StringIter(*this, (Uint)str_.size()); }

	// Get the characters at the beginning and end of the string or
	// Char::eof() if the string is empty.
	Char front() const { return *begin(); }
	Char back() const;

	// Clear
	void clear() { str_.clear(); }

	// Size
	bool empty() const { return str_.empty(); }

	// Reserve space for at least "capacity" characters
	void reserve(Uint capacity) { str_.reserve(4 * capacity); }

	// Append. If a single character then must not be EOF.
	String& operator+=(const String& s) { str_ += s.str_; return *this; }
	String& operator+=(Char ch);

	// Concatenation
	String operator+(const String& s) const { String ret = *this; ret += s; return ret; }
	String operator+(Char ch) const { String ret = *this; ret += ch; return ret; }

	// Find first. The returned string positions point to the beginning and one beyond
	// the end of the first occurrence of a substring which matches the argument.
	// If the argument is not found then the return value is (end(), end()).
	StringIterPair findFirst(const String& str) const;

	// Find first. The returned string iterator points to the first character
	// which matches the argument or end() if not found.
	StringIter findFirst(Char ch) const;

	// Find last. The returned string positions point to the beginning and one beyond
	// the end of the last occurrence of a substring which matches the argument.
	// If the argument is not found then the return value is (end(), end()).
	StringIterPair findLast(const String& str) const;

	// Find last. The returned string iterator points to the last character
	// which matches the argument or end() if not found.
	StringIter findLast(Char ch) const;

	// Comparison
	bool operator==(const String& other) const { return str_ == other.str_; }
	bool operator!=(const String& other) const { return str_ != other.str_; }
	bool operator<(const String& other) const { return str_ < other.str_; }
	bool operator<=(const String& other) const { return str_ <= other.str_; }
	bool operator>(const String& other) const { return str_ > other.str_; }
	bool operator>=(const String& other) const { return str_ >= other.str_; }

	// Output as UTF8.
	std::string toUtf8() const { return str_; }

	// Output to a platform string so that it can be used by the underlying
	// operating system.
	PlatformString toPlatform() const;
	
	// Convert string to uppercase
	String toUpperCopy() const;

	// Convert string to lowercase
	String toLowerCopy() const;

	// Case insensitive equality comparison between strings
	bool caselessEquals(const String& other) const;

	// Test whether the string begins with the given string
	bool beginsWith(const String& other) const;

	// Test whether the string ends with the given string
	bool endsWith(const String& other) const;

	// Test whether the string begins with the given string
	// using a caseless comparison
	bool caselessBeginsWith(const String& other) const;

	// Trim whitespace to the right and left.
	String trimCopy() const;

	// Get a hash of the string for use in unordered_map or
	// unordered_set.
	size_t hash() const { return std::hash<std::string>()(str_); }

private:
	// Check that the UTF8 string supplied as input is valid and assert if not.
	static void validate(const std::string& s);

	// Check that the UTF8 string supplied as input is valid and assert if not.
	// Returns the number of bytes in the string. 
	static Uint validate(const char* s);

private:
	friend class StringIter;
	std::string str_;
};

// Allow streaming to an OutputStream object.
inline OutputStream& operator<<(OutputStream& os, const String& s) {
	for (StringIter pos = s.begin(); !pos.atEnd(); pos++) {
		os.streamChar(*pos);
	}
	return os;
}

// Hash for a string for use in unordered_map or unordered_set.
class StringHash {
public:
	StringHash() { }
	size_t operator()(const String& str) const {
		return str.hash();
	}
};
