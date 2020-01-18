#include "Def.hpp"
#include "MakeGen/Project.hpp"
#include "MakeGen/ProjectRegistry.hpp"
#include "MakeGen/ResourcePath.hpp"
#include "MakeGen/String.hpp"

Project::Project(
		const std::string& project, 
		bool isLiveNotDead, 
		std::initializer_list<std::string> dependencies) 
	:
	SRC_(Dir::SRC(), project),
	BUILD_(Dir::BUILD(), project),
	TEST_(Dir::TEST(), project),
	project_(project),
	allSrcFiles_(SRC_.allFilesRecursive()),
	script_()
{
	if (isLiveNotDead) {
		ProjectRegistry::instance().addLiveProject(this);
		for (const auto& dependency : dependencies) {
			ProjectRegistry::instance().addDependency(this, dependency);
		}
	}
	else {
		ProjectRegistry::instance().addDeadProject(this);
		ASSERT(dependencies.size() == 0);
	}
}

std::string Project::name() const {
	return project_;
}

const FileList& Project::allSrcFiles() const {
	return allSrcFiles_;
}

void Project::make(Script& script) const {
	// Save the script class so we do not need to keep passing it around.
	script_ = &script;

	// Banner
	script_->blankLine();
	script_->echo("**************************************************");
	script_->echo(String("Building ") << project_);
	script_->echo("**************************************************");

	// Ensure that the Build and Test directories exist.
	script_->createDir(BUILD_);
	script_->createDir(TEST_);

	// Delete all files in the Build and Test directories.
	script_->removeDirContents(BUILD_);
	script_->removeDirContents(TEST_);

	// Call the project implementation to create the project-specific
	// output.
	makeProject();

	// Create _MakeSuccess.txt.
	script_->copyFile(
		File(Dir::SRC("MakeGen"), MAKE_SUCCESS_TXT),
		File(BUILD_, MAKE_SUCCESS_TXT));

	script_ = 0;
}

FileList Project::allSrcCpps() const {
	FileList ret = allSrcFiles_;
	ret.removeUnlessExt("cpp");
	return ret;
}

void Project::createDir(const Dir& dir) const {
	script_->createDir(dir);
}

void Project::compileAndLink(
	const std::string& outStem,
	const FileList& cppFiles, 
	const FileList& libFiles, 
	CompilerOptionsSelect cppOptsSel, 
	LinkerOptionsSelect linkOptsSel) const
{
	FileList linkFiles;
	compile(cppFiles, cppOptsSel, linkFiles);
	linkFiles.add(libFiles);
	link(outStem, linkFiles, linkOptsSel);
}

File Project::compileResource(
	const File& rcFile,
	ResourceOptionsSelect rcOptsSel) const 
{
	// No need to echo the source name - the compiler does that.
	File resFile = File(BUILD_, rcFile.stem(), "res");
	script_->compileResource(rcFile, resFile, rcOptsSel);
	return resFile;
}

void Project::runExecutable(
	const std::string& stem,
	const std::string& cmdLineOptions) const
{
	runExecutable(stem, stem, cmdLineOptions);
}

void Project::runExecutable(
	const std::string& exeStem,
	const std::string& logStem,
	const std::string& cmdLineOptions) const
{
	File exeFile(BUILD_, exeStem, "exe");
	File logFile(BUILD_, RUN_LOG_STEM_PREFIX + logStem, RUN_LOG_EXT);
	script_->echo(String("Running ") << exeFile.name() << " " << cmdLineOptions);
	script_->executeFile(exeFile, logFile, cmdLineOptions);
}

void Project::compile(
	const FileList& cppFiles,
	CompilerOptionsSelect cppOptsSel,
	FileList& objFiles) const
{
	for (const auto& cppFile : cppFiles) {
		objFiles.add(compile(cppFile, cppOptsSel));
	}
}

File Project::compile(
	const File& cppFile,
	CompilerOptionsSelect cppOptsSel) const
{
	// No need to echo the source name - the compiler does that.
	File objFile = File(BUILD_, cppFile.stem(), "obj");
	script_->compile(cppFile, objFile, cppOptsSel);
	return objFile;
}

File Project::link(
	const std::string& stem,
	const FileList& linkFiles,
	LinkerOptionsSelect linkOptsSel) const
{
	const char* ext = 0;
	switch (linkOptsSel) {
	case LOS_EXE_STD: ext = "exe"; break;
	case LOS_EXE_STACK: ext = "exe"; break;
	case LOS_DLL_STD: ext = "dll"; break;
	case LOS_LIB_STD: ext = "lib"; break;
	default: FAIL;
	}

	File outFile = File(BUILD_, stem, ext);
	script_->echo(String("Linking ") << outFile.name());
	script_->link(outFile, linkFiles, linkOptsSel);

	return outFile;
}

void Project::runBatch(
	const File& batchFile,
	const File& logFile,
	const std::string& cmdLineOptions) const
{
	script_->echo(String("Running ") << batchFile.name() << " " << cmdLineOptions);
	script_->executeFile(batchFile, logFile, cmdLineOptions);
}

void Project::copyFiles(
	const FileList& srcFiles,
	const Dir& dstDir) const
{
	script_->createDir(dstDir);
	for (const File& f : srcFiles) {
		script_->copyFile(f, File(dstDir, f.name()));
	}
}
