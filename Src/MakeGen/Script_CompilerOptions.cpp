#include <sstream>
#include "MakeGen/ResourcePath.hpp"
#include "MakeGen/Script.hpp"
#include "MakeGen/String.hpp"

// The following table documents the usage of compiler options by
// this make utility and Visual Studio. The "Make" column shows make
// usage, the "MSVS" column shows Visual Studio usage and the "Default"
// column shows the default Visual Studio setting for a new project.
// A blank entry means all columns are the same as the leftmost one.
// A "------" entry means not allowed or applicable. A "?" character
// is a single character wildcard. Exceptions are given in the
// description.
// 
// Defines beginning with EXT_ are project specific (external defines).
//
// The compiler does not use PDB files to save time and this requires
// the changes to /Fd, /Gm, /ZI and /Zi.
//
//                            Make   MSVS   Default
//                            ====== ====== ======
// /analyze-                  Msv32?               Do not analyse
// /bigobj                    ------ ------ ------ Increase object file sections.
// /c                         Msv???               Compile only (implicit in default build)
// /D_CONSOLE                 ------ Msv??? Msv??? Define for console application
// /D_DEBUG                   Msv??D               Define to detect debug build
// /D_LIB                     Msv???               Define for .lib (NOTE: Makefile does not define)
// /D_UNICODE                 Msv???               Define for Unicode support
// /DEXT_BATCH_MAKE           ?????? ------ ------ Flag to indicate a batch file make (i.e. not a Visual Studio build)
// /DEXT_BUILD_TOOL_MSV       Msv??? Msv??? ------ Build name starts with Msv e.g. Msv32D     
// /DEXT_BUILD_TOOL_GNU       ------               Build name starts with Gnu e.g. Gnu32D     
// /DEXT_BUILD_SIZE_32        ???32? ???32? ------ Build is 32 bit e.g. Msv32D     
// /DEXT_BUILD_SIZE_64        ???64? ???64? ------ Build is 64 bit e.g. Msv64D     
// /DEXT_BUILD_TYPE_DEBUG     ?????D ?????D ------ Build is debug e.g. Msv64D     
// /DEXT_BUILD_TYPE_RELEASE   ?????R ?????R ------ Build is release e.g. Msv64R     
// /DNOMINMAX                 ??????               Do not define min and max macros in Windows.h.
// /DNDEBUG                   Msv??R               Define to detect release build
// /DUNICODE                  Msv???               Define for Unicode support
// /DWIN32                    Msv32?               Windows 32 bit build (not to be confused with _WIN32 which is valid for x64 too)
// /diagnostics:classic       Msv???               Classic diagnostic with just line number
// /EHsc                      Msv???               Exception handling model
// /errorReport:prompt        ------ ------ Msv??? Prompt for error reports to MS
// /errorReport:none          Msv??? Msv??? ------ No error reports to MS
// /Fa<dir>                   ------ ------ Msv??? Assembler listing output directory (Cannot disable in MSVS build)
// /FC                        Msv??? Msv??? ------ Display full path of source files
// /Fd<file>                  ------ Msv??? Msv??? Program database file
// /Fo<dir/>                  Msv???               Output object file to supplied directory
// /Fp<file>                  ------ ------ Msv??? Precompiled header file
// /fp:precise                Msv???               Floating point behaviour control
// /Gd                        Msv???               cdecl calling convention
// /GL                        Msv??R               Whole program optimisation
// /Gm                        ------ Msv??D Msv??D Enable minimal rebuild
// /Gm-                       Msv??? Msv??R Msv??R Do not enable minimal rebuild
// /GS                        Msv???               Buffers security check
// /Gy                        Msv??R               Enable function level linking
// /I<path>                   Msv???               Include path
// /MD                        ------ ------ Msv??R Release with MS runtime library in DLL 
// /MDd                       ------ ------ Msv??D Debug with MS runtime library in DLL
// /MT                        Msv??R Msv??R ------ Release with MS runtime library statically linked
// /MTd                       Msv??D Msv??D ------ Debug with MS runtime library statically linked
// /nologo                    Msv???               No banner
// /O2                        Msv??R               Create fast code
// /Od                        Msv??D               Disable optimisation
// /Oi                        Msv??R               Generate intrinsic functions
// /Oy-                       Msv32?               Add frame pointer?
// /RTC1                      Msv??D               Enables run time error checking
// /sdl                       Msv???               Runtime security checks
// /W3                        Msv???               Warning level 3
// /WX-                       ------ ------ Msv??? Do not treat warnings as errors
// /WX                        Msv??? Msv??? ------ Treat warnings as errors
// /Yu<file>                  ------ ------ Msv??? Use precompiled header file
// /Zc:forScope               Msv???               Enforce standard behaviour
// /Zc:inline                 Msv???               Enforce standard behaviour
// /Zc:wchar_t                Msv???               Enforce standard behaviour
// /ZI                        ------ Msv??D Msv??D Produce PDB and support edit and continue
// /Zi                        ------ Msv??R Msv??R Produce PDB

void Script::compilerOptionsPreamble() {
	for (int i = 0; i < 2; ++i) {
		bool is32Not64 = (i == 0);
#if defined(EXT_BUILD_SIZE_32)
		// A 32 bit build never needs to build anything as 64 bit.
		if (!is32Not64) {
			continue;
		}
#endif

		os_ << "set CLOPTS" << (is32Not64 ? "32" : "64") << "=";
#if defined(EXT_BUILD_TOOL_MSV)
		os_ << "/c"
			" /D_UNICODE"
			" /DEXT_BATCH_MAKE"
			" /DEXT_BUILD_TOOL_MSV"
			" /DNOMINMAX"
			" /DUNICODE"
			" /diagnostics:classic"
			" /EHsc"
			" /errorReport:none"
			" /FC"
			" /fp:precise"
			" /Gd"
			" /Gm-"
			" /GS"
			" /I" << Dir::SRC() << "\\"
			" /nologo"
			" /sdl"
			" /W3"
			" /WX"
			" /Zc:forScope"
			" /Zc:inline"
			" /Zc:wchar_t";
#elif defined(EXT_BUILD_TOOL_GNU)
		os_ << "/DEXT_BUILD_TOOL_GNU";
#error "GNU not supported yet"
#else
#error "Illegal build tool"
#endif

		if (is32Not64) {
			os_ << " /analyze-"
				" /DEXT_BUILD_SIZE_32"
				" /DWIN32"
				" /Oy-";
		}
		else {
			os_ << " /DEXT_BUILD_SIZE_64";
		}

#if defined(EXT_BUILD_TYPE_DEBUG)
		os_ << " /D_DEBUG"
			" /DEXT_BUILD_TYPE_DEBUG"
			" /MTd"
			" /Od"
			" /RTC1";
#elif defined(EXT_BUILD_TYPE_RELEASE)
		os_ << " /DEXT_BUILD_TYPE_RELEASE"
			" /DNDEBUG"
			" /GL"
			" /Gy"
			" /MT"
			" /O2"
			" /Oi";
#else
#error "Illegal build type"
#endif

		os_ << std::endl;
	}
}

void Script::compilerOptionsOutput(const Dir& dstDir, CompilerOptionsSelect cos) { 
	os_ << "%CLOPTS" << ((bss_ == BSS_32_BIT) ? "32" : "64") << "%";
	os_ << " /Fo" << dstDir << "\\";

	if (cos == COS_BIG) {
		os_ << " /bigobj";
	}
}
