#pragma once
#include <filesystem>
#include <ostream>
#include <set>
#include <string>
#include "MakeGen/Def.hpp"
#include "MakeGen/Dir.hpp"

namespace fs = std::experimental::filesystem::v1;

// Class which represents a file path. The file consists of a directory
// and a filename.
class File {
public:
	// Construct a file with the given name in the given directory.
	// The name must be of the form "stem.ext" where stem is the file
	// name stem and ext is the extension. The stem does not contain
	// '.'.
	File(const Dir& dir, const std::string& name);

	// Same but with the name and extension set separately.
	File(const Dir& dir, const std::string& stem, const std::string& ext);

	// Get the directory part of the file path.
	Dir dir() const;

	// Get the file name - i.e. name.ext.
	std::string name() const;

	// Get the file stem
	std::string stem() const;

	// Get the file extension
	std::string ext() const;

	// Comparison - needed to store in a set.
	bool operator<(const File& other) const;

	// Returns true if the file exists
	bool exists() const;

	// Returns the last write time for the file. Set to fs::file_time_type()
	// if the file does not exist.
	fs::file_time_type lastWriteTime() const;

	// Rename the file to one with the given name and extension. The current file
	// must exist. Both old and new files must be based on the Build or Src
	// directories.
	void rename(const File& newFile) const;

	// Get the file path as a string relative to the code root directory.
	// The path separator will be \ for Windows and / for Linux.
	std::string str() const;

	// Get the directory path as an absolute fs path.
	fs::path getFsPath() const;

private:

	// Check that the stem name of a file is valid.
	// We allow alphanumerics plus '_' or '-'.
	static void checkFileStem(const std::string& stem);

	// Check that the extension of a file is valid.
	// We allow alphanumerics plus '_' or '-'. Also
	// '.' provided it is not the first or last
	// character.
	static void checkFileExt(const std::string& ext);

private:
	// The parent directory.
	Dir dir_;

	// The file name stem.
	std::string stem_;

	// The file name extension.
	std::string ext_;

	// Make a friend to allow output.
	friend std::ostream& operator<<(std::ostream& os, const File& file);
};

// Output the file as a relative path beginning with Src, Build or Test.
std::ostream& operator<<(std::ostream& os, const File& file);

// Class which represents an unsorted unique list of files.
class FileList {
public:
	// Create an empty file list
	FileList();

	// Create a file list from a list of files.
	FileList(std::initializer_list<File> files);

	// True if the list is empty.
	bool empty() const;

	// Add to the file list. It is not an error if the file already exists - the
	// new file is just ignored.
	void add(const File& file);
	void add(const FileList& fileList);

	// Remove from the file list. It is not an error if the file does not exist.
	void remove(const File& file);
	void remove(const FileList& fileList);
	void remove(const Dir& dir);

	// Remove from the file list any files which do not have the given extension.
	// The argument should omit the ".".
	void removeUnlessExt(const std::string& ext);

	// Iteration
	typedef std::set<File>::const_iterator const_iterator;
	const_iterator begin() const;
	const_iterator end() const;

private:
	// The file list.
	std::set<File> files_;
};

// Output a space separated list of files.
std::ostream& operator<<(std::ostream& os, const FileList& fileList);

// Some standard files and directories
const File TEST_TOOL_LIB(Dir::BUILD("TestTool"), "TestTool.lib");
const File UTIL_LIB(Dir::BUILD("Util"), "Util.lib");
