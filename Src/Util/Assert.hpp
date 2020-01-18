#pragma once
#include <cstdint>
#include "Util/Def.hpp"

// Needed to prevent warnings about numerical overflow in
// constexpr expressions.
#pragma warning(disable: 4307)

namespace AssertHash {

	// Local constant expression to lower case.
	inline constexpr char toLower(char c) {
		return ((c >= 'A') && (c <= 'Z')) ? (c - 'A' + 'a') : c;
	}

	// We use the Fowler–Noll–Vo hash function on 32 bits (see Wikipedia)
	const std::uint32_t FNV_PRIME = 16777619u;
	const std::uint32_t FNV_OFFSET_BASIS = 2166136261u;

	// Get a pointer to the '\0' string terminator at the end of
	// a const char* string s. Note that we include the '\0' byte
	// in the hash to make life easier in case the input string is empty.
	inline constexpr const char* const end(const char* const s) {
		return (*s == '\0') ? s : end(s + 1);
	}

	// The Fowler–Noll–Vo hashing function.
	inline constexpr std::uint32_t fnvHash(std::uint32_t oldHash, char data) {
		return (oldHash * FNV_PRIME) ^ (unsigned char)data;
	}

	// Get the hash of all the characters from position p back to begin but
	// stopping on any directory separator (\ / :).
	inline constexpr std::uint32_t cumulativeHash(const char* const begin, const char* const p) {
		return
			(*p == '\\')	? FNV_OFFSET_BASIS :
			(*p == '/')		? FNV_OFFSET_BASIS :
			(*p == ':')		? FNV_OFFSET_BASIS :
			(p == begin)	? fnvHash(FNV_OFFSET_BASIS, toLower(*p)) :
							  fnvHash(cumulativeHash(begin, p - 1), toLower(*p));
	}

	// Complete hash of the filename file and line number line but ignoring
	// any directories in file.
	inline constexpr std::uint32_t fileLineHash(const char* const s, int line) {
		return
			fnvHash(
				fnvHash(
					fnvHash(
						fnvHash(cumulativeHash(s, end(s)), (char)(line & 0xff)),
						(char)((line >> 8) & 0xff)),
					(char)((line >> 16) & 0xff)),
				(char)((line >> 24) & 0xff));
	}
}

// Test the condition x. If it is true then do nothing.
// If it is false then call the handling function which
// does not return. 
#define ASSERT(x) do { \
	if (!(x)) { \
		doAssertFail(AssertHash::fileLineHash(__FILE__, __LINE__)); \
	} \
} while (false)

// Test the condition x but only in the debugging version.
#if BUILD(DEBUG)
#define DEBUG_ASSERT(x) ASSERT(x)
#else
#define DEBUG_ASSERT(x) do ; while (false)
#endif

// Unconditional failure. Call the handling function which
// typically does not return.
#define FAIL { \
	doAssertFail(AssertHash::fileLineHash(__FILE__, __LINE__)); \
}

// Implementation function for ASSERT and FAIL. This must be supplied
// by every executable as a customised function. It must not return.
[[noreturn]] void doAssertFail(std::uint32_t fileLineHash);
