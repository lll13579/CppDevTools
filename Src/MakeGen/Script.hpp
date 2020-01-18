#pragma once
#include <sstream>
#include "MakeGen/Def.hpp"
#include "MakeGen/Dir.hpp"
#include "MakeGen/File.hpp"
#include "MakeGen/String.hpp"

// Selection of build size. By default the correct compiler tools are
// selected in make.bat depending on the supplied build option. However
// the occasional project may require both 32 and 64 bit versions to be
// built. This is selected by choosing an enumeration value.
enum BuildSizeSelect {
	BSS_32_BIT,				// 32 bit build size
	BSS_64_BIT,				// 64 bit build size
#if defined(EXT_BUILD_SIZE_32)
	BSS_DEFAULT = BSS_32_BIT,
#elif defined(EXT_BUILD_SIZE_64)
	BSS_DEFAULT = BSS_64_BIT,
#else
#error "Illegal build size"
#endif
};

// Selection of a particular compiler option set.
enum CompilerOptionsSelect {
	// The standard compiler options
	COS_STD,
	// Same as COS_STD but set the big object file option to allow
	// the compiler to cope with large object files.
	COS_BIG,
};

// Selection of a particular resource compiler option set.
enum ResourceOptionsSelect {
	// The standard resource compiler options
	ROS_STD,
};

// Selection of a particular linker option set.
enum LinkerOptionsSelect {
	// The standard linker options for linking an EXE.
	LOS_EXE_STD,
	// The standard linker options for linking an EXE but
	// with a larger stack of 5MB rather than the 1MB default.
	LOS_EXE_STACK,
	// The standard linker options for linking a DLL.
	LOS_DLL_STD,
	// The standard linker options for linking an LIB.
	LOS_LIB_STD,
};

// Class responsible for output all the commands to the batch file.
// Later this could be made virtual with different implementations
// for Windows, Linux etc.
class Script {
public:
	// Constructor.
	Script(std::ostream& os);

	// Output a command to set an environment variable with the given
	// name to the directory with the given path.
	void setEnvironmentDirectory(const std::string& name, const Dir& dir);

	// Output a command to set an environment variable with the given
	// name to the file with the given path.
	void setEnvironmentFile(const std::string& name, const File& file);

	// Output the compiler options preamble. Just once is needed
	// for the entire output file.
	void compilerOptionsPreamble();

	// Output the resource compiler options preamble. Just once is needed
	// for the entire output file.
	void resourceOptionsPreamble();

	// Output the linker options preamble. Just once is needed
	// for the entire output file.
	void linkerOptionsPreamble();

	// Output a blank line to the output file.
	void blankLine();

	// Echo a string to an output line. The string s cannot be empty
	// (echo does not work if so - it prints out the echo status).
	void echo(const std::string& s);

	// Create a directory unless it already exists. The directory must
	// be the Build or Test directory or a sub-directory.
	void createDir(const Dir& dir);

	// Remove all the contents of a directory (but not the directory itself).
	// The directory must be the Build or Test directory or a sub-directory.
	void removeDirContents(const Dir& dir);

	// Copy a file. The destination must be in the Build or Test directory
	// trees.
	void copyFile(const File& srcFile, const File& dstFile);

	// Delete the given file which must be in the Build or Test directory
	// tree.
	void removeFile(const File& file);

	// Execute a file.
	void executeFile(
		const File& exeFile,
		const File& logFile,
		const std::string& cmdLineOptions);

	// Compile
	void compile(
		const File& cppFile,
		const File& objFile,
		CompilerOptionsSelect cppOptsSel);

	// Compile a resource file
	void compileResource(
		const File& rcFile, 
		const File& resFile,
		ResourceOptionsSelect rcOptsSel);

	// Link
	void link(
		const File& outFile,
		const FileList& linkFiles,
		LinkerOptionsSelect linkOptsSel);

	// Successful exit.
	void successfulExit();

private:
	// Check that a file or directory is based in the Build or Test
	// directory trees.
	void checkBuildOrTest(const File& file);
	void checkBuildOrTest(const Dir& dir);

	// Output compiler options in environment variables CLOPTS32 and CLOPTS64.
	void compilerOptionsOutput(const Dir& dstDir, CompilerOptionsSelect cos);

	// Output the resource compiler options in environment variables RCOPTS32
	// and RCOPTS64.
	void resourceOptionsOutput(const File& dstFile, ResourceOptionsSelect ros);

	// Output the linker options in environment variables LINKOPTS32 and
	// LINKOPTS64.
	void linkerOptionsOutput(const File& dstFile, LinkerOptionsSelect los);

	// Exit if there has been an error.
	void exitOnError();

private:
	// The build size
	BuildSizeSelect bss_;

	// The output stream.
	std::ostream& os_;
};
