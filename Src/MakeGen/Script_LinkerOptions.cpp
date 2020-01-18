#include <sstream>
#include "MakeGen/Script.hpp"
#include "MakeGen/String.hpp"

// The following table documents the usage of linker options. See CompilerOptions.cpp
// for a description of the columns.

// The options /DEBUG, /PDB and /INCREMENTAL are not used to save time.

//                            Make   MSVS   Default
//                            ====== ====== ======
// /DEBUG                     ------ Msv??? Msv??? Create debugging information
// /DEBUG:NONE                Msv??? ------ ------ Do not create debugging information
// /DYNAMICBASE               Msv???               Allow random rebasing
// ERRORREPORT:NONE           Msv??? Msv??? ------ No error reports to MS
// ERRORREPORT:PROMPT         ------ ------ Msv??? Prompt for error reports to MS
// /INCREMENTAL               ------ Msv??D Msv??D Incremental linking
// /INCREMENTAL:NO            Msv??? Msv??R Msv??R No incremental linking
// /LTCG                      Msv??R ------ ------ Link time code generation (an optimisation)
// /LTCG:incremental          ------ Msv??R Msv??R Link time code generation for files which have changed (an optimisation)
// /MACHINE:X86               Msv32?               Target machine X86
// /MACHINE:X64               Msv64?               Target machine X64
// /MANIFEST                  Msv???               Create a separate manifest file rather than embed it
// /ManifestFile:<file>       Msv???               Manifest file
// /MANIFESTUAC:"level='asInvoker' uiAccess='false'"
//                            Msv???               Default user access control settings for manifest
// /NOLOGO                    Msv???               No banner
// /NXCOMPAT                  Msv???               Mark the executable as compatible with Windows DEP
// /OPT:ICF                   Msv??R               Optimisation control
// /OPT:REF                   Msv??R               Optimisation control
// /OUT:<file>                Msv???               Output filename
// /PDB:<file>                ------ Msv??? Msv??? Program database file
// /PGD:<file>                ------ ------ Msv??? Program guided optimisation file
// /SAFESEH                   Msv32R               Safe exception handlers
// /SUBSYSTEM:CONSOLE         Msv???               Console application
// /TLBID:1                   Msv???               Resource ID for type library
// /WX                        Msv??? Msv??? ------ Treat warnings as errors

void Script::linkerOptionsPreamble() {
	for (int i = 0; i < 2; ++i) {
		bool is32Not64 = (i == 0);
#if defined(EXT_BUILD_SIZE_32)
		// A 32 bit build never needs to build anything as 64 bit.
		if (!is32Not64) {
			continue;
		}
#endif

		os_ << "set LINKOPTS" << (is32Not64 ? "32" : "64") << "=";
#if defined(EXT_BUILD_TOOL_MSV)
		os_ << "/DEBUG:NONE"
			" /DYNAMICBASE"
			" /ERRORREPORT:NONE"
			" /INCREMENTAL:NO"
			" /MANIFEST"
			" /MANIFESTUAC:\"level='asInvoker' uiAccess='false'\""
			" /NOLOGO"
			" /NXCOMPAT"
			" /SUBSYSTEM:CONSOLE"
			" /TLBID:1"
			" /WX"
			" kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib";
#elif defined(EXT_BUILD_TOOL_GNU)
#error "GNU not supported yet"
#else
#error "Illegal build tool"
#endif

		if (is32Not64) {
			os_ << " /MACHINE:X86";
#if defined(EXT_BUILD_TYPE_RELEASE)
			os_ << " /SAFESEH";
#endif
		}
		else {
			os_ << " /MACHINE:X64";
		}

#if defined(EXT_BUILD_TYPE_DEBUG)
#elif defined(EXT_BUILD_TYPE_RELEASE)
		os_ << " /LTCG"
			" /OPT:ICF"
			" /OPT:REF";
#else
#error "Illegal build type"
#endif

		os_ << std::endl;
	}
}

void Script::linkerOptionsOutput(const File& dstFile, LinkerOptionsSelect los) {
	// Libraries do not use %LINKOPTS%.
	if (los != LOS_LIB_STD) {
		os_ << "%LINKOPTS" << ((bss_ == BSS_32_BIT) ? "32" : "64") << "%";
	}

	switch (los) {
	case LOS_EXE_STD:
		ASSERT(dstFile.ext() == "exe");
		os_ << " /ManifestFile:" << dstFile << ".intermediate.manifest"
			<< " /OUT:" << dstFile;
		break;
	case LOS_EXE_STACK:
		ASSERT(dstFile.ext() == "exe");
		os_ << " /STACK:0x500000"
			<< " /ManifestFile:" << dstFile << ".intermediate.manifest"
			<< " /OUT:" << dstFile;
		break;
	case LOS_DLL_STD:
		ASSERT(dstFile.ext() == "dll");
		os_ << " /DLL"
			<< " /IMPLIB:" << File(dstFile.dir(), dstFile.stem(), "lib")
			<< " /ManifestFile:" << dstFile << ".intermediate.manifest"
			<< " /OUT:" << dstFile;
		break;
	case LOS_LIB_STD:
		ASSERT(dstFile.ext() == "lib");
		os_ << "/OUT:" << dstFile
			<< " /NOLOGO"
			<< " /WX";
		if (bss_ == BSS_32_BIT) {
			os_ << " /MACHINE:X86";
		}
		else {
			os_ << " /MACHINE:X64";
		}
#if defined(EXT_BUILD_TYPE_DEBUG)
#elif defined(EXT_BUILD_TYPE_RELEASE)
		os_ << " /LTCG";
#else
#error "Illegal build type"
#endif
		break;
	default:
		FAIL;
	}
}
