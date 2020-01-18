#pragma once
#include <filesystem>
#include <initializer_list>
#include <ostream>
#include <set>
#include <string>
#include <vector>
#include "MakeGen/Def.hpp"

class DirList;
class DirListIterator;
class FileList;

namespace fs = std::experimental::filesystem::v1;

// Class which represents a directory. Directories are in a tree
// which has a base in either Src, Build or Test.
class Dir {
public:
	// Construct a directory from a parent and subdirectory.
	Dir(const Dir& parent, const std::string& subdir);

	// Return the Src directory.
	static Dir SRC();

	// Return the Build/<xxxxx> directory. e.g. Build/Msv32D.
	static Dir BUILD();

	// Return the Test/<xxxxx> directory. e.g. Test/Msv32D.
	static Dir TEST();

	// The same as above but with an argument of a project sub-directory.
	static Dir SRC(const std::string& projectName);
	static Dir BUILD(const std::string& projectName);
	static Dir TEST(const std::string& projectName);

	// Compose a directory from a parent and subdirectory.
	Dir operator/(const std::string& subdir) const;

	// Test what the root directory is.
	bool baseIsSrc() const;
	bool baseIsBuild() const;
	bool baseIsTest() const;

	// Get the parent directories of the final directory. Not allowed
	// If this directory is at the base level (i.e. Src, Build/<xxxxx>
	// or Test/<xxxxx>).
	Dir parent() const;

	// Get the leaf directory name - i.e. without any parent directories.
	std::string name() const;

	// Get all the sub-directories of this directory. The base directory must
	// be the source directory. The ".vs" directory is ignored.
	DirList allSubdirs() const;

	// Get all the files in this directory. The base directory must
	// be the source directory.
	FileList allFiles() const;

	// Get all the files in this directory including in all subdirectories.
	// The base directory must be the source directory. The ".vs" directory
	// is ignored.
	FileList allFilesRecursive() const;

	// Comparison - needed to store in a set.
	bool operator<(const Dir& other) const;

	// Comparison needs to check fotr equality
	bool operator==(const Dir& other) const;
	bool operator!=(const Dir& other) const;

	// Create this directory if it does not already exist. The base directory
	// must be Build or Test
	void create();

	// Remove the contents of this directory. The base directory must be Build
	// or Test. Create the directory if it does not exist.
	void removeContents();

	// Get the directory path as a string relative to the base directory.
	// The path separator will be \ for Windows and / for Linux.
	std::string str() const;

	// Get the directory path as an absolute fs path.
	fs::path getFsPath() const;

private:

	// The base directory type.
	enum BaseType {
		BASE_SRC,		// Root is Src
		BASE_BUILD,		// Root is Build
		BASE_TEST		// Root is Test
	};

private:

	// Constructors.
	Dir(BaseType rootType);

	// Check that the name of a subdirectory is valid.
	// We allow alphanumerics plus '_' or '-'.
	static void checkDirName(const std::string& subdir);

	// Get the parent of the base directory as an absolute fs path.
	static fs::path getBaseParent();

private:
	// Root type
	BaseType baseType_;

	// The directory tree below the root. May be empty.
	std::vector<std::string> dirNames_;

	// Friends
	friend std::ostream& operator<<(std::ostream& os, const Dir& dir);
	friend class File;
};

// Output the directory as a relative path beginning with Src, Build or Test.
std::ostream& operator<<(std::ostream& os, const Dir& dir);

// Class which represents an unsorted unique list of directories.
class DirList {
public:
	// Construct an empty directory list
	DirList();

	// Construct a directory list from a list of directories.
	DirList(std::initializer_list<Dir> dirs);

	// True if the list is empty.
	bool empty() const;

	// Add to the directory list. It is not an error if the directory already
	// exists - the new directory is just ignored.
	void add(const Dir& dir);
	void add(const DirList& dirList);

	// Remove from the directory list. It is not an error if the directory
	// does not exist.
	void remove(const Dir& dir);
	void remove(const DirList& dirList);

	// Iteration
	typedef std::set<Dir>::const_iterator const_iterator;
	const_iterator begin() const;
	const_iterator end() const;

private:
	// The file list.
	std::set<Dir> dirs_;
};

// Output a space separated list of directories.
std::ostream& operator<<(std::ostream& os, const DirList& dirList);
