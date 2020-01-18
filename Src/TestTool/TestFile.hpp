#pragma once
#include <string>
#include "Util/File.hpp"

// In the comments below <build> is Msv64D, Gnu32R etc. depending
// on the build.

// The following also work with a virtual file. In this case the directories
// for the virtual file are handled conventionally - so for example createBinaryTestFile
// will ensure that all parent directories exist. But the actual file will be
// virtual and not stored in the filesystem.
namespace TestFile {
	// Get the directory path for a source input directory.
	// The supplied path is relative to the ".../Git/.../Src" directory.
	// An argument of "" returns this Src directory itself.
	// The path need not exist (but it will in practice).
	DirPath getSrcDir(const String& relPath);

	// Get the file path for a source input file.
	// The supplied path is relative to the ".../Git/.../Src" directory.
	// An argument of "" is not allowed.
	// The path need not exist.
	FilePath getSrcFile(const String& relPath);

	// Get the directory path for a build directory.
	// The supplied path is relative to the ".../Git/.../Build/<build>" directory.
	// An argument of "" returns this directory itself.
	// The path need not exist.
	DirPath getBuildDir(const String& relPath);

	// Get the file path for a build file.
	// The supplied path is relative to the ".../Git/.../Build/<build>" directory.
	// An argument of "" is not allowed.
	// The path need not exist.
	FilePath getBuildFile(const String& relPath);

	// Create the supplied directory and any parent directories which do not already exist.
	// The directory must be either the build directory or one of its children.
	// It is not an error if the directory already exists.
	// The path relPath is relative to the ".../Git/.../Build/<build>" directory.
	DirPath createBuildDir(const String& relPath);
	void createBuildDir(const DirPath& dir);

	// Create an encoded build file including any parent directories which do not
	// already exist. The file must be in the build directory or one of its children.
	// The contents are stored in the file.
	// If the file already exists it is overwritten.
	// The path relPath is relative to the ".../Git/.../Build/<build>" directory.
	FilePath createEncodedBuildFile(CharEncoding::Ordinal encoding, const String& relPath, const String& contents);
	void createEncodedBuildFile(CharEncoding::Ordinal encoding, const FilePath& file, const String& contents);

	// Get the directory path for a test directory.
	// The supplied path is relative to the ".../Git/.../Test/<build>" directory.
	// An argument of "" returns this directory itself.
	// The path need not exist.
	DirPath getTestDir(const String& relPath);

	// Get the file path for a test file.
	// The supplied path is relative to the ".../Git/.../Test/<build>" directory.
	// An argument of "" is not allowed.
	// The path need not exist.
	FilePath getTestFile(const String& relPath);

	// Create the supplied directory and any parent directories which do not already exist.
	// The directory must be either the test directory or one of its children.
	// It is not an error if the directory already exists.
	// The path relPath is relative to the ".../Git/.../Test/<build>" directory.
	DirPath createTestDir(const String& relPath);
	void createTestDir(const DirPath& dir);

	// Create a binary test file including any parent directories which do not
	// already exist. The file must be in the test directory or one of its children.
	// The contents are stored in the file.
	// If the file already exists it is overwritten.
	// The path relPath is relative to the ".../Git/.../Test/<build>" directory.
	FilePath createBinaryTestFile(const String& relPath, const std::string& contents);
	FilePath createBinaryTestFile(const String& relPath, const char* contents);
	void createBinaryTestFile(const FilePath& file, const std::string& contents);
	void createBinaryTestFile(const FilePath& file, const char* contents);

	// Create an encoded test file including any parent directories which do not
	// already exist. The file must be in the test directory or one of its children.
	// The contents are stored in the file.
	// If the file already exists it is overwritten.
	// The path relPath is relative to the ".../Git/.../Test/<build>" directory.
	FilePath createEncodedTestFile(CharEncoding::Ordinal encoding, const String& relPath, const String& contents);
	void createEncodedTestFile(CharEncoding::Ordinal encoding, const FilePath& file, const String& contents);

	// Read a binary file into a string.
	// The file directory must be either the source directory or test directory
	// or one of their children. The file must exist.
	std::string readBinaryFile(const FilePath& file);

	// Read an encoded file into a string.
	// Newline combinations in the input are converted to \n.
	// The file directory must be either the source directory or test directory
	// or one of their children. The file must exist.
	String readEncodedFile(CharEncoding::Ordinal encoding, const FilePath& file);
}
