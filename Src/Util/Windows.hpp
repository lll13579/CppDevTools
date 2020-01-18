#pragma once
// This file is a wrapper around Windows.h which counteracts
// those settings which will cause problems elsewhere.
// Define WIN32_FAT_AND_FLABBY if the full Windows header
// is required otherwise only the lean and mean header is used.

#include "Util/Def.hpp"
#if BUILD(WINDOWS)

// This macro stops Windows.h from defining "min" and "max" and
// thereby messing up std::min and std::max. It should be defined
// anyway globally since Poco header files also need it.
#ifndef NOMINMAX
#define NOMINMAX
#endif

// See above
#ifndef WIN32_FAT_AND_FLABBY
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

// Undefine macros which we use elsewhere.
#undef SEVERITY_ERROR
#undef ERROR

#endif
