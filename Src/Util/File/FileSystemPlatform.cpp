#include <filesystem>
#include <fstream>
#include "Util/Assert.hpp"
#include "Util/File.hpp"
#include "Util/File/FileRawInput.hpp"
#include "Util/File/FileRawOutput.hpp"
#include "Util/File/FileSystemPlatform.hpp"

#if BUILD(WINDOWS)
namespace fs = std::experimental::filesystem::v1;
#else
#error TODO
#endif

///////////////////////////////////////////////////////////////////////////////
// Local
///////////////////////////////////////////////////////////////////////////////

// Convert a std::filesystem file path to a String handling
// non-ASCII characters correctly.
static String fsToString(const fs::path& fsPath) {
	std::string s = fsPath.u8string();
	return String(s);
}

// Convert a String to a std::filesystem file path handling
// non-ASCII characters correctly.
static fs::path stringToFs(const String& strPath) {
	return fs::u8path(strPath.toUtf8());
}

///////////////////////////////////////////////////////////////////////////////
// PlatformFileRawInput
///////////////////////////////////////////////////////////////////////////////

class PlatformFileRawInput : public FileRawInput {
public:
	PlatformFileRawInput(const FilePath& path) :
		FileRawInput(),
		fin_(),
		fail_(false),
		atEof_(false)
	{
		try {
			fin_.open(path.str().toPlatform(), std::ios::binary);
			if (!fin_ || !fin_.is_open()) {
				fail_ = true;
			}
		}
		catch (...) {
			fail_ = true;
		}
	}
	~PlatformFileRawInput() {
	}
	Uint read(char* dst, Uint size) {
		if (fail_ || atEof_ || (size == 0)) {
			return 0;
		}

		Uint readSize = 0;
		try {
			std::filebuf* rdbuf = fin_.rdbuf();
			readSize = (Uint)rdbuf->sgetn(dst, size);
			ASSERT(readSize <= size);
			if (readSize == 0) {
				atEof_ = true;
			}
			if (!fin_) {
				fail_ = true;
				readSize = 0;
			}
		}
		catch (...) {
			fail_ = true;
			readSize = 0;
		}
		return readSize;
	}
	bool failed() {
		return fail_;
	}
private:
	std::ifstream fin_;
	bool fail_;
	bool atEof_;
};

///////////////////////////////////////////////////////////////////////////////
// PlatformFileRawOutput
///////////////////////////////////////////////////////////////////////////////

class PlatformFileRawOutput : public FileRawOutput {
public:
	PlatformFileRawOutput(const FilePath& path) :
		FileRawOutput(),
		fout_(),
		fail_(false)
	{
		try {
			fout_.open(path.str().toPlatform(), std::ios::binary);
			if (!fout_ || !fout_.is_open()) {
				fail_ = true;
			}
		}
		catch (...) {
			fail_ = true;
		}
	}
	~PlatformFileRawOutput() {
		close();
	}
	void write(const char* src, Uint size) {
		if (fail_ || (size == 0)) {
			return;
		}

		Uint writeSize = 0;
		try {
			std::filebuf* rdbuf = fout_.rdbuf();
			writeSize = (Uint)rdbuf->sputn(src, size);
			if (!fout_ || (writeSize != size)) {
				fail_ = true;
			}
		}
		catch (...) {
			fail_ = true;
		}
	}
	void close() {
		if (!fail_) {
			try {
				fout_.close();
			}
			catch (...) {
				fail_ = true;
			}
		}
	}
	bool failed() {
		return fail_;
	}
private:
	std::ofstream fout_;
	bool fail_;
};

///////////////////////////////////////////////////////////////////////////////
// FileSystemPlatform
///////////////////////////////////////////////////////////////////////////////

FileSystemPlatform::FileSystemPlatform() :
	workingDirIsInitialised_(false),
	workingDir_()
{
}

FileSystemPlatformPtr FileSystemPlatform::instance() {
	static FileSystemPlatformPtr ret = std::make_shared<FileSystemPlatform>();
	return ret;
}

DirPath FileSystemPlatform::getWorkingDir() {
	if (!workingDirIsInitialised_) {
		workingDirIsInitialised_ = true;
		try {
			fs::path curPath = fs::current_path();
			if (curPath != fs::path()) {
				DirPath workingDir(fsToString(curPath));
				// We insist that the working directory exists
				if (workingDir.exists()) {
					workingDir_ = workingDir;
				}
			}
		}
		catch (...) {
		}
	}
	return workingDir_;
}

String FileSystemPlatform::makePath(const String& absPath) {
	try {
		// Get std path
		fs::path bfp = stringToFs(absPath);

		if (bfp.is_absolute()) {
			// Convert to preferred form (i.e. use backslash in Windows)
			bfp.make_preferred();
			return fsToString(bfp);
		}
	}
	catch (...) {
	}
	return String();
}

String FileSystemPlatform::makePath(const String& path, const DirPath& baseDir) {
	try {
		// Get fs path
		fs::path bfpDir = stringToFs(path);

		// Combine paths to result
		fs::path bfp = fs::absolute(bfpDir, stringToFs(baseDir.str()));

		// Convert to preferred form (i.e. use backslash in Windows)
		bfp.make_preferred();

		// Result
		return fsToString(bfp);
	}
	catch (...) {
	}
	return String();
}

bool FileSystemPlatform::dirCreate(const DirPath& path) {
	if (!path.isValid()) {
		return false;
	}
	else {
		fs::path bfp = stringToFs(path.str());
		fs::create_directories(bfp);
		return dirExists(path);
	}
}

bool FileSystemPlatform::dirExists(const DirPath& path) {
	if (!path.isValid()) {
		return false;
	}
	else {
		fs::path bfp = stringToFs(path.str());
		return fs::exists(bfp) && fs::is_directory(bfp);
	}
}

bool FileSystemPlatform::fileExists(const FilePath& path) {
	if (!path.isValid()) {
		return false;
	}
	else {
		fs::path bfp = stringToFs(path.str());
		return fs::exists(bfp) && fs::is_regular_file(bfp);
	}
}

DirPath FileSystemPlatform::parentDir(const String& path) {
	if (path.empty()) {
		return DirPath();
	}
	else {
		try {
			fs::path bfpPath = stringToFs(path);
			fs::path bfpParent = bfpPath.parent_path();
			return DirPath(true, fsToString(bfpParent));
		}
		catch (...) {
			return DirPath();
		}
	}
}

String FileSystemPlatform::leafName(const String& path) {
	if (path.empty()) {
		return String();
	}
	else {
		try {
			fs::path bfpPath = stringToFs(path);
			fs::path leafPath = bfpPath.filename();
			return fsToString(leafPath);
		}
		catch (...) {
			return String();
		}
	}
}

FileRawInputPtr FileSystemPlatform::openForInput(const FilePath& path) {
	return std::make_shared<PlatformFileRawInput>(path);
}

FileRawOutputPtr FileSystemPlatform::openForOutput(const FilePath& path) {
	return std::make_shared<PlatformFileRawOutput>(path);
}

void FileSystemPlatform::listFileNames(const DirPath& dir, std::set<String>& fileNames, bool dirsAlso) const {
	if (!dir.exists()) {
		return;
	}
	fs::path bfp = stringToFs(dir.str());
	ASSERT(fs::exists(bfp));
	ASSERT(fs::is_directory(bfp));
	
    for (fs::directory_iterator it(bfp); it != fs::directory_iterator(); ++it) {
		fs::path filePath = it->path();
		if (fs::exists(filePath) && 
			(fs::is_regular_file(filePath) ||
			 (dirsAlso && fs::is_directory(filePath)))) 
		{
			fs::path leafPath = filePath.filename();
			fileNames.insert(fsToString(leafPath));
		}
	}
}
