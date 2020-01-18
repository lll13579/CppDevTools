#pragma once
#include <memory>
#include <set>
#include "Util/Def.hpp"
#include "Util/File.hpp"

DPTR(FileRawInput)
DPTR(FileRawOutput)
DPTR(FileSystemPlatform)

// The file system for the platform controls the normal conventional file system.
// The implementation is the only part of the code which uses the standard
// filesystem library.
//
// We do not make member functions static to force the initialisation
// of the file system.
class FileSystemPlatform {
private:
	// Initialise
	ALLOW_MAKE_SHARED(FileSystemPlatform);
	FileSystemPlatform();

public:
	// Singleton instance
	static FileSystemPlatformPtr instance();

	// Get the current working directory.
	// Returns DirPath() if there is an error.
	DirPath getWorkingDir();

	// Make a file system path in standard form from another absolute path.
	// The path need not exist. If the path is invalid or relative then
	// returns is "". Example "/a/b/../c" would get converted to "C:\a\c". 
	String makePath(const String& absPath);

	// Make a file system path in standard form from an absolute or relative
	// path. If the path is relative then the base directory path is used.
	// None of the paths need exist. If the path is invalid or if the path
	// is relative and the base directory is invalid then returns "".
	// Example "a/b", "/c" would get converted to "C:\c\a\b".
	String makePath(const String& path, const DirPath& baseDir);

	// Create the directory and any parent directories which do not already exist.
	// It is not an error if the directory already exists. Returns true
	// on success, false on error.
	bool dirCreate(const DirPath& path);

	// Returns true if the given directory path is valid, exists and is a directory.
	bool dirExists(const DirPath& path);

	// Returns true if the given file path is valid, exists and is a file.
	bool fileExists(const FilePath& path);

	// Get the parent directory of the given path or DirPath()
	// if there is an error or there is no such directory.
	DirPath parentDir(const String& path);

	// Get the final leaf in the path i.e. a name
	// without any / or \ separators. Returns "" if there is
	// an error. Does not work with Unicode yet - see FileTest.cpp.
	String leafName(const String& path);

	// Opens a platform file for raw binary input.
	FileRawInputPtr openForInput(const FilePath& path);

	// Opens a platform file for raw binary output.
	FileRawOutputPtr openForOutput(const FilePath& path);

	// List all the file names in the given directory
	// (i.e. the leaf names not the full path). If dirsAlso
	// is true then sub-directories are included too.
	void listFileNames(const DirPath& dir, std::set<String>& fileNames, bool dirsAlso) const;

private:
	bool workingDirIsInitialised_;
	DirPath workingDir_;
};

