#pragma once
#include <cstdint>
#include <string>

// Common definitions

// Datetime
typedef std::uint32_t Datetime;

// The name of the build
#if defined(EXT_BUILD_TOOL_MSV)
#define BUILD_NAME_1 "Msv"
#elif defined(EXT_BUILD_TOOL_GNU)
#error "GNU not supported yet"
#else
#error "Illegal build tool"
#endif
#if defined(EXT_BUILD_SIZE_32)
#define BUILD_NAME_2 "32"
#elif defined(EXT_BUILD_SIZE_64)
#define BUILD_NAME_2 "64"
#else
#error "Illegal build size"
#endif
#if defined(EXT_BUILD_TYPE_DEBUG)
#define BUILD_NAME_3 "D"
#elif defined(EXT_BUILD_TYPE_RELEASE)
#define BUILD_NAME_3 "R"
#else
#error "Illegal build type"
#endif
#define BUILD_NAME BUILD_NAME_1 BUILD_NAME_2 BUILD_NAME_3

// Assertion
class AssertException {
public:
	AssertException() {
		int breakpoint = 0;
		++breakpoint;
	}
};
#define ASSERT(x) do { if (!(x)) throw AssertException(); } while (false)
#define FAIL do { throw AssertException(); } while (false)
