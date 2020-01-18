#pragma once
#include <cstring>
#include <filesystem>
#include "TestTool/TestFile.hpp"
#include "Util/Assert.hpp"
#include "Util/Char.hpp"
#include "Util/CharEncoding.hpp"
#include "Util/Def.hpp"
#include "Util/String.hpp"

#if BUILD(WINDOWS)
namespace fs = std::experimental::filesystem::v1;
#else
#error TODO
#endif

namespace TestFile {

	///////////////////////////////////////////////////////////////////////////////
	// Local
	///////////////////////////////////////////////////////////////////////////////
	// This is a duplicate of FileSystemPlatform.cpp

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
	// Global
	///////////////////////////////////////////////////////////////////////////////

	// Checks that the given absolute path has the given parent.
	// If allow equality is true then also returns true if the two paths are the
	// same.
	static bool isParentDir(const DirPath& parent, const String& absPath, bool allowEquality) {
		String sParent = parent.str();
		if (sParent == absPath) {
			return allowEquality;
		}
		else {
			std::string sParentSlash = sParent.toUtf8() + FileSystem::getPathSeparator().toUtf8();
			return sParentSlash == absPath.toUtf8().substr(0, sParentSlash.size());
		}
	}
			
	// Get the code base directory - i.e. as in the path:
	// .../Src/TestTool/Impl/TestFile.cpp
	static DirPath getCodeDirBase() {
		static DirPath ret;
		if (!ret.isValid()) {
			fs::path thisFileWithWrongCase = __FILE__;
		
			// Hack to get the correct case for the file. We assume that each
			// directory (and the initial drive letter) has an initial capital
			// with the rest in lower case apart from the TestTool directory.
			fs::path thisFile;
			{
				std::string s = thisFileWithWrongCase.string();
				bool toCaps = true;
				for (std::string::size_type i = 0; i < s.size(); ++i) {
					if ((s[i] == '\\') || (s[i] == '/')) {
						toCaps = true;
					}
					else if (toCaps) {
						s[i] = toupper(s[i]);
						toCaps = false;
						if (s.substr(i, 8) == "Testtool") {
							s[i + 4] = 'T';
						}
					}
				}
				thisFile = s;
			}

			fs::path testFileDir = thisFile.parent_path();
			ASSERT(!testFileDir.empty());
			ASSERT(testFileDir.is_absolute());
			ASSERT(fs::is_directory(testFileDir));
			ASSERT(fsToString(testFileDir.filename()) == "Impl");
		
			fs::path testUtilDir = testFileDir.parent_path();
			ASSERT(!testUtilDir.empty());
			ASSERT(fs::is_directory(testUtilDir));
			ASSERT(fsToString(testUtilDir.filename()) == "TestTool");
		
			fs::path srcDir = testUtilDir.parent_path();
			ASSERT(!srcDir.empty());
			ASSERT(fs::is_directory(srcDir));
			ASSERT(fsToString(srcDir.filename()) == "Src");
		
			fs::path codeDir = srcDir.parent_path();
			ASSERT(!codeDir.empty());
			ASSERT(fs::is_directory(codeDir));
		
			ret = DirPath(fsToString(codeDir));
		}
		return ret;
	}

	// Get the Src base directory
	static DirPath getSrcDirBase() {
		static DirPath ret;
		if (!ret.isValid()) {
			fs::path codeDir = stringToFs(getCodeDirBase().str());
		
			fs::path srcDir = codeDir / "Src";
			ASSERT(fs::is_directory(srcDir));
		
			ret = DirPath(fsToString(srcDir));
		}
		return ret;
	}

	DirPath getSrcDir(const String& relPath) {
		if (relPath.empty()) {
			return getSrcDirBase();
		}
		else {
			ASSERT(stringToFs(relPath).is_relative());
			return DirPath(relPath, getSrcDirBase());
		}
	}

	FilePath getSrcFile(const String& relPath) {
		ASSERT(!relPath.empty());
		ASSERT(stringToFs(relPath).is_relative());
		return FilePath(relPath, getSrcDirBase());
	}

	// Get the Build/<build> base directory
	static DirPath getBuildDirBase() {
		static DirPath ret;
		if (!ret.isValid()) {
			fs::path codeDir = stringToFs(getCodeDirBase().str());
		
			fs::path buildDir = codeDir / "Build";
			ASSERT(fs::is_directory(buildDir));

			fs::path buildDir2 = buildDir / BUILD_DIRECTORY;
			ASSERT(fs::is_directory(buildDir2));

			ret = DirPath(fsToString(buildDir2));
		}
		return ret;
	}

	DirPath getBuildDir(const String& relPath) {
		if (relPath.empty()) {
			return getBuildDirBase();
		}
		else {
			ASSERT(stringToFs(relPath).is_relative());
			return DirPath(relPath, getBuildDirBase());
		}
	}

	FilePath getBuildFile(const String& relPath) {
		ASSERT(!relPath.empty());
		ASSERT(stringToFs(relPath).is_relative());
		return FilePath(relPath, getBuildDirBase());
	}

	DirPath createBuildDir(const String& relPath) {
		DirPath ret = getBuildDir(relPath);
		ASSERT(ret.isValid());
		createBuildDir(ret);
		return ret;
	}

	void createBuildDir(const DirPath& dir) {
		DirPath baseDir = getBuildDirBase();
		ASSERT(isParentDir(baseDir, dir.str(), true));
		fs::create_directories(stringToFs(dir.str()));
	}

	FilePath createEncodedBuildFile(CharEncoding::Ordinal encoding, const String& relPath, const String& contents) {
		FilePath ret = getBuildFile(relPath);
		ASSERT(ret.isValid());
		createEncodedBuildFile(encoding, ret, contents);
		return ret;
	}

	void createEncodedBuildFile(CharEncoding::Ordinal encoding, const FilePath& file, const String& contents) {
		createBuildDir(file.getParentDir());
		FileEncodedOutputPtr fout = FileEncodedOutput::create(encoding, file);
		ASSERT(fout);
		fout->write(contents);
		fout->close();
	}

	// Get the Test/<build> base directory
	static DirPath getTestDirBase() {
		static DirPath ret;
		if (!ret.isValid()) {
			fs::path codeDir = stringToFs(getCodeDirBase().str());
			fs::path testDir = codeDir / "Test";
			fs::path buildDir = testDir / BUILD_DIRECTORY;
			ret = DirPath(fsToString(buildDir));
		}
		return ret;
	}

	DirPath getTestDir(const String& relPath) {
		if (relPath.empty()) {
			return getTestDirBase();
		}
		else {
			ASSERT(stringToFs(relPath).is_relative());
			return DirPath(relPath, getTestDirBase());
		}
	}

	FilePath getTestFile(const String& relPath) {
		ASSERT(!relPath.empty());
		ASSERT(stringToFs(relPath).is_relative());
		return FilePath(relPath, getTestDirBase());
	}

	DirPath createTestDir(const String& relPath) {
		DirPath ret = getTestDir(relPath);
		ASSERT(ret.isValid());
		createTestDir(ret);
		return ret;
	}

	void createTestDir(const DirPath& dir) {
		DirPath baseDir = getTestDirBase();
		ASSERT(isParentDir(baseDir, dir.str(), true));
		fs::create_directories(stringToFs(dir.str()));
	}

	FilePath createBinaryTestFile(const String& relPath, const std::string& contents) {
		FilePath ret = getTestFile(relPath);
		ASSERT(ret.isValid());
		createBinaryTestFile(ret, contents);
		return ret;
	}

	FilePath createBinaryTestFile(const String& relPath, const char* contents) {
		FilePath ret = getTestFile(relPath);
		ASSERT(ret.isValid());
		createBinaryTestFile(ret, contents);
		return ret;
	}

	void createBinaryTestFile(const FilePath& file, const std::string& contents) {
		createTestDir(file.getParentDir());
		FileBinaryOutputPtr fout = FileBinaryOutput::create(file);
		ASSERT(fout);
		fout->write(contents);
		fout->close();
	}

	void createBinaryTestFile(const FilePath& file, const char* contents) {
		createTestDir(file.getParentDir());
		FileBinaryOutputPtr fout = FileBinaryOutput::create(file);
		ASSERT(fout);
		Uint contentsLen = (Uint)strlen(contents);
		fout->write(contents, contentsLen);
		fout->close();
	}

	FilePath createEncodedTestFile(CharEncoding::Ordinal encoding, const String& relPath, const String& contents) {
		FilePath ret = getTestFile(relPath);
		ASSERT(ret.isValid());
		createEncodedTestFile(encoding, ret, contents);
		return ret;
	}

	void createEncodedTestFile(CharEncoding::Ordinal encoding, const FilePath& file, const String& contents) {
		createTestDir(file.getParentDir());
		FileEncodedOutputPtr fout = FileEncodedOutput::create(encoding, file);
		ASSERT(fout);
		fout->write(contents);
		fout->close();
	}

	std::string readBinaryFile(const FilePath& file) {
		ASSERT(isParentDir(getSrcDirBase(), file.str(), false) ||
			   isParentDir(getTestDirBase(), file.str(), false));
		ASSERT(file.exists());
		FileBinaryInputPtr in = FileBinaryInput::open(file);
		return in->readString();
	}

	String readEncodedFile(CharEncoding::Ordinal encoding, const FilePath& file) {
		ASSERT(isParentDir(getSrcDirBase(), file.str(), false) ||
			   isParentDir(getTestDirBase(), file.str(), false));
		ASSERT(file.exists());
		FileEncodedInputPtr in = FileEncodedInput::open(encoding, file);
		return in->readString();
	}

}

