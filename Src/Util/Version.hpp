#pragma once

// Convert argument to string
#define STRINGIZE2(x) #x
#define STRINGIZE(x) STRINGIZE2(x)

// The minor or major version number must change if new features are added.
// The revision version number increments for bug fixes.
// The version year is used in copyright messages.
// Version numbers must be between 0 and 999 inclusive.
#define VERSION_MAJOR		1
#define VERSION_MINOR		0
#define VERSION_REVISION	4
#define VERSION_YEAR		2018

// As strings
#define VERSION_MAJOR_STRING		STRINGIZE(VERSION_MAJOR)
#define VERSION_MINOR_STRING		STRINGIZE(VERSION_MINOR)
#define VERSION_REVISION_STRING		STRINGIZE(VERSION_REVISION)
#define VERSION_YEAR_STRING			STRINGIZE(VERSION_YEAR)

// Standard version string
#define VERSION_STRING VERSION_MAJOR_STRING "." VERSION_MINOR_STRING "." VERSION_REVISION_STRING

// Standard calculation to compress a version down into a single Uint32
#define VERSION_COMBO (1000000 * VERSION_MAJOR + 1000 * VERSION_MINOR + VERSION_REVISION)

// Version string used as part of a path
#define VERSION_PATH VERSION_MAJOR_STRING "_" VERSION_MINOR_STRING "_" VERSION_REVISION_STRING
