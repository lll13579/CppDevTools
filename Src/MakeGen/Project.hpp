#pragma once
#include <memory>
#include <ostream>
#include <string>
#include "MakeGen/Def.hpp"
#include "MakeGen/File.hpp"
#include "MakeGen/Script.hpp"

// Macros for defining a project. Usage:
//		// Normal project
//		DEFINE_LIVE_PROJECT(MyProjName, { Dependency list }) {
//			// Code to output to script_.
//			...
//		}
//
//		// Project which is dead and not built.
//		DEFINE_DEAD_PROJECT(MyProjName) {
//		}
//
// Use variadic macros to allow passing of comma in initialiser. The
// variadic argument is the dependency list.
#define DEFINE_LIVE_PROJECT(project, ...) DEFINE_PROJECT(project, true, __VA_ARGS__)
#define DEFINE_DEAD_PROJECT(project) DEFINE_PROJECT(project, false, { })
#define DEFINE_PROJECT(project, isLiveNotDead, ...) \
class project ## Project : public Project { \
public: \
	project ## Project() : \
		Project(#project, isLiveNotDead, __VA_ARGS__) \
		{ } \
	void makeProject() const; \
}; \
static project ## Project project ## Instance; \
void project ## Project :: makeProject() const

// A project corresponds to a subdirectory of Src. The project automatically
// has a dependency on all the files within this sub-directory including
// within nested subdirectories.
class Project {
public:
	// Constructor. "project" is the name of the project. isLiveNotDead
	// is true for a normal project and false for one which should be
	// ignored. The dependencies are a list of dependent project names.
	// These should include:
	// o Projects which are needed for linking.
	// o Projects which have header files which are #included.
	// o Projects which are needed for running.
	Project(
		const std::string& project, 
		bool isLiveNotDead, 
		std::initializer_list<std::string> dependencies);

	// Get the name of the project
	std::string name() const;

	// Get a list of all the files in the source directory (including in
	// subdirectories).
	const FileList& allSrcFiles() const;

	// Make the project and stream it to the supplied script
	// object.
	void make(Script& script) const;

	// Virtual call which outputs to the script.
	virtual void makeProject() const = 0;

protected:

	// Get all the source .cpp files (including in subdirectories).
	FileList allSrcCpps() const;

	// Create a directory unless it already exists. The directory must
	// be the Build or Test directory or a sub-directory.
	void createDir(const Dir& dir) const;

	// Compile a list of source files and link it with optional
	// libraries. The linker options determine whether the output
	// is an executable, DLL or static library.
	// outStem is the output filename stem.
	void compileAndLink(
		const std::string& outStem,
		const FileList& cppFiles, 
		const FileList& libFiles, 
		CompilerOptionsSelect cppOptsSel = COS_STD, 
		LinkerOptionsSelect linkOptsSel = LOS_EXE_STD) const;

	// Compile a resource .rc file to a .res file. Returns the .res file.
	File compileResource(
		const File& rcFile,
		ResourceOptionsSelect rcOptsSel = ROS_STD) const;

	// Run an executable file. The file has a .exe extension
	// and is in the project build directory. A log file will
	// be named after the executable and output to the same
	// directory.
	void runExecutable(
		const std::string& stem,
		const std::string& cmdLineOptions = "") const;
	
	// The same as the above except the .exe and log file
	// name stems can be different.
	void runExecutable(
		const std::string& exeStem,
		const std::string& logStem,
		const std::string& cmdLineOptions) const;

	// Compile a list of files
	void compile(
		const FileList& cppFiles,
		CompilerOptionsSelect cppOptsSel,
		FileList& objFiles) const;

	// Compile a single file. The return value is the object file.
	File compile(
		const File& cppFile,
		CompilerOptionsSelect cppOptsSel) const;

	// Link object files. The return value is the output file.
	File link(
		const std::string& stem,
		const FileList& linkFiles,
		LinkerOptionsSelect linkOptsSel) const;

	// Run a batch file
	void runBatch(
		const File& batchFile,
		const File& logFile,
		const std::string& cmdLineOptions = "") const;

	// Copy a set of files in a file list to a target directory.
	// If the target directory does not exist it will be created.
	void copyFiles(
		const FileList& srcFiles,
		const Dir& dstDir) const;

protected:
	// The source directory for the project i.e. Src/<Project>.
	const Dir SRC_;

	// The build directory for the project i.e. Build/Msv???/<Project>.
	// A project may temporarily change this directory: it is used to
	// store all object files, link output files and run log files.
	// If changed it should be changed back again afterwards.
	mutable Dir BUILD_;

	// The test directory for the project i.e. Test/Msv???/<Project>.
	const Dir TEST_;

	// Project name
	const std::string project_;

	// All the files within the project source directory.
	const FileList allSrcFiles_;

	// Saved script class: only valid during running make().
	mutable Script* script_;
};
