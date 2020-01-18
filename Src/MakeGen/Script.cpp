#include "MakeGen/Script.hpp"
#include "MakeGen/ResourcePath.hpp"
#include "MakeGen/String.hpp"

Script::Script(std::ostream& os) :
	bss_(BSS_DEFAULT),
	os_(os)
{
}

void Script::setEnvironmentDirectory(
	const std::string& name,
	const Dir& dir)
{
	fs::path fp = dir.getFsPath();
	fp.make_preferred();
	os_ << "set "
		<< name
		<< "="
		<< fp.string()
		<< std::endl;
}

void Script::setEnvironmentFile(
	const std::string& name,
	const File& file)
{
	fs::path fp = file.getFsPath();
	fp.make_preferred();
	os_ << "set "
		<< name
		<< "="
		<< fp.string()
		<< std::endl;
}

// See Script_CompilerOptions.cpp for compilerOptionsPreamble.

// See Script_ResourceOptions.cpp for resourceOptionsPreamble.

// See Script_LinkerOptions.cpp for linkerOptionsPreamble.

void Script::blankLine() {
	os_ << std::endl;
}

void Script::echo(const std::string& s) {
	ASSERT(!s.empty());
	os_ << "echo " << s << std::endl;
}

void Script::createDir(const Dir& dir) {
	os_ << "mkdir " << dir << " >nul 2>&1" << std::endl;
	// Do not exit on error - there will be an error if the
	// directory already exists which is fine.
}

void Script::removeDirContents(const Dir& dir) {
	os_ << "del /s /q " << dir << "\\*.* >nul 2>&1" << std::endl;
	exitOnError();
}

void Script::copyFile(const File& srcFile, const File& dstFile) {
	// Do not use "copy" since this does not update the modification time.
	os_ << "type " << srcFile << " >" << dstFile << std::endl;
	exitOnError();
}

void Script::removeFile(const File& file) {
	os_ << "del /q " << file << " >nul 2>&1" << std::endl;
	exitOnError();
}

void Script::executeFile(
	const File& exeFile,
	const File& logFile,
	const std::string& cmdLineOptions)
{
	os_ << exeFile << " " << cmdLineOptions << " >" << logFile << " 2>&1" << std::endl;
	exitOnError();
}

void Script::compile(
	const File& cppFile,
	const File& objFile,
	CompilerOptionsSelect cppOptsSel)
{
	os_ << "cl ";
	compilerOptionsOutput(objFile.dir(), cppOptsSel);
	os_ << " " << cppFile << std::endl;
	exitOnError();
}

void Script::compileResource(
	const File& rcFile, 
	const File& resFile,
	ResourceOptionsSelect rcOptsSel)
{
	os_ << "rc ";
	resourceOptionsOutput(resFile, rcOptsSel);
	os_ << " " << rcFile << std::endl;
	exitOnError();
}

void Script::link(
	const File& outFile,
	const FileList& linkFiles,
	LinkerOptionsSelect linkOptsSel)
{
	os_ << ((linkOptsSel == LOS_LIB_STD) ? "lib " : "link ");
	linkerOptionsOutput(outFile, linkOptsSel);
	os_ << " " << linkFiles << std::endl;
	exitOnError();
}

void Script::successfulExit() {
	blankLine();
	echo("MakeGen successful!");
	os_ << "exit /b 0" << std::endl;
}

void Script::checkBuildOrTest(const File& file) {
	checkBuildOrTest(file.dir());
}

void Script::checkBuildOrTest(const Dir& dir) {
	ASSERT(dir.baseIsBuild() || dir.baseIsTest());
}

// See Script_CompilerOptions.cpp for Script::compilerOptionsOutput.

// See Script_ResourceOptions.cpp for Script::resourceOptionsOutput.

// See Script_LinkerOptions.cpp for linkerOptionsOutput.

void Script::exitOnError() {
	os_ << "if errorlevel 1 exit /b 1" << std::endl;
}
