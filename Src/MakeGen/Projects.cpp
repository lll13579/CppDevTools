#include "MakeGen/Dir.hpp"
#include "MakeGen/File.hpp"
#include "MakeGen/Project.hpp"

// This project is dependent on just about everything as it needs
// updating whenever the position of an ASSERT or FAIL changes.
// So add a dependency on all projects.
DEFINE_LIVE_PROJECT(AssertHashMap, { 
	"TestTool",
	"TestToolTest",
	"Util",
	"UtilTest" })
{
	FileList cppFiles = allSrcCpps();
	FileList libFiles{ TEST_TOOL_LIB, UTIL_LIB };
	compileAndLink(project_, cppFiles, libFiles);
	runExecutable(project_);
}

DEFINE_DEAD_PROJECT(CheckLineEndings) {
}

DEFINE_DEAD_PROJECT(Make) {
}

DEFINE_DEAD_PROJECT(MakeGen) {
}

DEFINE_LIVE_PROJECT(
	TestTool,
	{ "Util" }) 
{
	FileList cppFiles = allSrcCpps();
	FileList libFiles;
	compileAndLink(project_, cppFiles, libFiles, COS_STD, LOS_LIB_STD);
};

DEFINE_LIVE_PROJECT(
	TestToolTest,
	{ "TestTool", "Util" })
{
	FileList cppFiles = allSrcCpps();
	FileList libFiles{ TEST_TOOL_LIB, UTIL_LIB };
	compileAndLink(project_, cppFiles, libFiles);
	runExecutable(project_);
}

DEFINE_LIVE_PROJECT(
	Util,
	{ }) 
{
	FileList cppFiles = allSrcCpps();
	FileList libFiles;
	compileAndLink(project_, cppFiles, libFiles, COS_STD, LOS_LIB_STD);
};

DEFINE_LIVE_PROJECT(
	UtilTest,
	{ "TestTool", "Util" })
{
	FileList cppFiles = allSrcCpps();
	cppFiles.remove(SRC_ / "Resource");
	FileList libFiles{ TEST_TOOL_LIB, UTIL_LIB };
	compileAndLink(project_, cppFiles, libFiles);
	runExecutable(project_);
}
