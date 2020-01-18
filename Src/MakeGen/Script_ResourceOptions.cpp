#include <sstream>
#include "MakeGen/Script.hpp"
#include "MakeGen/String.hpp"

// The following table documents the usage of resource compiler options.
// See CompilerOptions.cpp for a description of the columns.

//                            Make   MSVS   Default
//                            ====== ====== ======
// /D_UNICODE                 Msv???               Define for Unicode support
// /DEXT_BATCH_MAKE           ?????? ------ ------ Flag to indicate a batch file make
// /DEXT_BUILD_TOOL_MSV       Msv??? Msv??? ------ Build name starts with Msv e.g. Msv32D     
// /DEXT_BUILD_TOOL_GNU       ------               Build name starts with Gnu e.g. Gnu32D     
// /DEXT_BUILD_SIZE_32        ???32? ???32? ------ Build is 32 bit e.g. Msv32D     
// /DEXT_BUILD_SIZE_64        ???64? ???64? ------ Build is 64 bit e.g. Msv64D     
// /DEXT_BUILD_TYPE_DEBUG     ?????D ?????D ------ Build is debug e.g. Msv64D     
// /DEXT_BUILD_TYPE_RELEASE   ?????R ?????R ------ Build is release e.g. Msv64R     
// /DUNICODE                  Msv???               Define for Unicode support
// /Fo<file>                  Msv???               Output file
// /I<path>                   Msv???               Include path
// /l"0x0409"                 ------ ------ ------ Language id United States
// /nologo                    Msv???               No banner

void Script::resourceOptionsPreamble() {
	for (int i = 0; i < 2; ++i) {
		bool is32Not64 = (i == 0);
#if defined(EXT_BUILD_SIZE_32)
		// A 32 bit build never needs to build anything as 64 bit.
		if (!is32Not64) {
			continue;
		}
#endif

		os_ << "set RCOPTS" << (is32Not64 ? "32" : "64") << "=";
#if defined(EXT_BUILD_TOOL_MSV)
		os_ << "/D_UNICODE"
			" /DEXT_BATCH_MAKE"
			" /DEXT_BUILD_TOOL_MSV"
			" /DUNICODE"
			" /l\"0x0409\""
			" /nologo";
#elif defined(EXT_BUILD_TOOL_GNU)
		os_ << "/DEXT_BUILD_TOOL_GNU";
#error "GNU not supported yet"
#else
#error "Illegal build tool"
#endif

		if (is32Not64) {
			os_ << " /DEXT_BUILD_SIZE_32";
		}
		else {
			os_ << " /DEXT_BUILD_SIZE_64";
		}

#if defined(EXT_BUILD_TYPE_DEBUG)
		os_ << " /DEXT_BUILD_TYPE_DEBUG";
#elif defined(EXT_BUILD_TYPE_RELEASE)
		os_ << " /DEXT_BUILD_TYPE_RELEASE";
#else
#error "Illegal build type"
#endif

		os_ << std::endl;
	}
}

void Script::resourceOptionsOutput(const File& dstFile, ResourceOptionsSelect ros) { 
	os_ << "%RCOPTS" << ((bss_ == BSS_32_BIT) ? "32" : "64") << "%";
	os_ << " /Fo" << dstFile;
	ASSERT(ros == ROS_STD);
}
