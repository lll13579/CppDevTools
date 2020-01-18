#pragma once
#include <cstdint>
#include <memory>
#include <string>

// Build options.
// The supplied argument must be one of the valid values otherwise there will be
// a compiler error.
#define BUILD(x) ((BUILD_ ## x) / (_BUILD_ ## x ## _VALID_))

// Operating system
// #if BUILD(WINDOWS)		// Windows (any variety)
// #if BUILD(LINUX)			// Linux (any variety)
#define _BUILD_WINDOWS_VALID_ 1		
#define _BUILD_LINUX_VALID_ 1		
#ifdef _WIN32
	// 32 or 64 bit Windows
	#define BUILD_WINDOWS 1
	#define BUILD_LINUX 0
#else
	#define BUILD_WINDOWS 0
	#define BUILD_LINUX 1
#endif

// Compiler type
// #if BUILD(MSV)		// MS Visual Studio
// #if BUILD(GNU)		// Gnu
#define _BUILD_MSV_VALID_ 1
#define _BUILD_GNU_VALID_ 1
#if defined(EXT_BUILD_TOOL_MSV)
	#if defined(EXT_BUILD_TOOL_GNU)
		#error "Incompatible build tool setting"
	#endif
	#define BUILD_MSV 1
	#define BUILD_GNU 0
#elif defined(EXT_BUILD_TOOL_GNU)
	#if !defined(__GNUC__)
		#error "Illegal Gnu compiler setting"
	#endif
	#define BUILD_MSV 0
	#define BUILD_GNU 1
#else
	#error "No EXT_BUILD_TOOL_xxx definition"
#endif

// 32 vs 64 bit build:
// #if BUILD(32_BIT)		// 32 bit build
// #if BUILD(64_BIT)		// 64 bit build
#define _BUILD_32_BIT_VALID_ 1
#define _BUILD_64_BIT_VALID_ 1
#if defined(EXT_BUILD_SIZE_32)
	#if defined(EXT_BUILD_SIZE_64)
		#error "Incompatible build size setting"
	#endif
	#define BUILD_32_BIT 1
	#define BUILD_64_BIT 0
#elif defined(EXT_BUILD_SIZE_64)
	#define BUILD_32_BIT 0
	#define BUILD_64_BIT 1
#else
	#error "No EXT_BUILD_SIZE_xx definition"
#endif

// Debug vs release build
// #if BUILD(DEBUG)			// Debug build
// #if BUILD(RELEASE)		// Release build
#define _BUILD_DEBUG_VALID_ 1
#define _BUILD_RELEASE_VALID_ 1
#if defined(EXT_BUILD_TYPE_DEBUG)
	#if defined(EXT_BUILD_TYPE_RELEASE)
		#error "Incompatible build type setting"
	#endif
	#define BUILD_DEBUG 1
	#define BUILD_RELEASE 0
#elif defined(EXT_BUILD_TYPE_RELEASE)
	#define BUILD_DEBUG 0
	#define BUILD_RELEASE 1
#else
	#error "No EXT_BUILD_TYPE_xxxx definition"
#endif
#if BUILD(WINDOWS) && BUILD(DEBUG) && (!defined(_DEBUG) || defined(NDEBUG))
	#error "Inconsistent Windows debug definitions
#endif
#if BUILD(WINDOWS) && BUILD(RELEASE) && (defined(_DEBUG) || !defined(NDEBUG))
	#error "Inconsistent Windows release definitions
#endif

// big vs little endian build:
// #if BUILD(BIG_ENDIAN)		// Big endian build
// #if BUILD(LITTLE_ENDIAN)		// Little endian build
#define _BUILD_BIG_ENDIAN_VALID_ 1
#define _BUILD_LITTLE_ENDIAN_VALID_ 1
#if BUILD(WINDOWS)
	#define BUILD_BIG_ENDIAN 0
	#define BUILD_LITTLE_ENDIAN 1
#elif BUILD(LINUX)
	#error TODO
#else
	#error "Illegal OS"
#endif

// Build directory name
// path = .../BUILD_DIRECTORY/...
#if BUILD(MSV) && BUILD(32_BIT) && BUILD(DEBUG)
	#define BUILD_DIRECTORY "Msv32D"
#elif BUILD(MSV) && BUILD(32_BIT) && BUILD(RELEASE)
	#define BUILD_DIRECTORY "Msv32R"
#elif BUILD(MSV) && BUILD(64_BIT) && BUILD(DEBUG)
	#define BUILD_DIRECTORY "Msv64D"
#elif BUILD(MSV) && BUILD(64_BIT) && BUILD(RELEASE)
	#define BUILD_DIRECTORY "Msv64R"
#elif BUILD(GNU) && BUILD(32_BIT) && BUILD(DEBUG)
	#define BUILD_DIRECTORY "Gnu32D"
#elif BUILD(GNU) && BUILD(32_BIT) && BUILD(RELEASE)
	#define BUILD_DIRECTORY "Gnu32R"
#elif BUILD(GNU) && BUILD(64_BIT) && BUILD(DEBUG)
	#define BUILD_DIRECTORY "Gnu64D"
#elif BUILD(GNU) && BUILD(64_BIT) && BUILD(RELEASE)
	#define BUILD_DIRECTORY "Gnu64R"
#else
	#error "Illegal build option combination"
#endif

// Macro for declaring a shared pointer
#define DPTR(x) class x; typedef std::shared_ptr<x> x ## Ptr;

// Macro for allowing a class to use std::make_shared for a private constructor.
// So within the declaration of class X add:
// ALLOW_MAKE_SHARED(X);
// See http://stackoverflow.com/questions/8147027/how-do-i-call-stdmake-shared-on-a-class-with-only-protected-or-private-const
#if BUILD(MSV)
	#define ALLOW_MAKE_SHARED(x) friend class std::_Ref_count_obj<x>
#elif BUILD(GNU)
	#define ALLOW_MAKE_SHARED(x) friend void __gnu_cxx::new_allocator<x>::construct<x>(x*)
#else
	#error Illegal build
#endif

// Undefined size signed and unsigned integer types. Use these for preference
// over C++ standard types when the size of the type is unimportant.
typedef int				Int;
typedef unsigned int	Uint;

// Fixed size signed and unsigned integer types. Use these for preference
// over the <cstdint> types since these should really always be qualified
// by std::.
typedef unsigned char	Uint8;
typedef std::uint16_t	Uint16;
typedef std::uint32_t	Uint32;
typedef std::uint64_t	Uint64;
typedef signed char		Int8;
typedef std::int16_t	Int16;
typedef std::int32_t	Int32;
typedef std::int64_t	Int64;

// The type for a line number
typedef Int32			Line;

// Floating point types
typedef float			Float32;
typedef double			Float64;
