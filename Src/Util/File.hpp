#pragma once
#include <limits>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include "Util/Char.hpp"
#include "Util/CharEncoding.hpp"
#include "Util/Def.hpp"
#include "Util/String.hpp"

DPTR(CharInputConverter)
DPTR(CharOutputConverter)
DPTR(FileBinaryInput)
DPTR(FileBinaryOutput)
DPTR(FileEncodedInput)
DPTR(FileEncodedOutput)
DPTR(FileRawInput)
DPTR(FileRawOutput)
DPTR(FileSystemErrorHandler)

// An absolute path to a filesystem directory.
// It is not necessary for the path to exist.
class DirPath {
public:
	// Construct an invalid directory path.
	DirPath();

	// Construct a directory path from an absolute directory path. The path
	// need not exist. If the path is invalid or relative then this object
	// is marked as invalid. 
	explicit DirPath(const String& absDirPath);

	// Construct a directory path from an absolute or relative directory
	// path. If the directory path is relative then the base directory
	// is used. None of the paths need exist. If the directory path is invalid
	// or if the directory path is relative and the base directory is invalid
	// then this object is marked as invalid. 
	DirPath(const String& dirPath, const DirPath& baseDir);

	// Returns the directory path as a string. This will be an absolute path
	// without any . or .. or other special characters and in a format
	// compatible with the native file system. Returns "" if the path is invalid.
	String str() const;

	// Test whether the path is valid - i.e. whether it was marked as valid
	// during construction.
	bool isValid() const;

	// Create the directory and any parent directories which do not already exist.
	// It is not an error if the directory already exists. Returns true if after
	// the call the directory exists.
	bool create() const;

	// Test whether the path is valid AND exists AND is a directory.
	bool exists() const;

	// Get the parent directory of this directory or DirPath()
	// if there is an error.
	DirPath getParentDir() const;

	// Get the final directory in the path i.e. a name
	// without any / or \ separators. Returns "" if there is
	// an error. 
	// Does not work with Unicode yet - see FileTest.cpp. But
	// since this call is only used for testing not a priority.
	String getLeafName() const;

	// Comparison
	bool operator==(const DirPath& other) const;
	bool operator!=(const DirPath& other) const;

	// If exists() is true then list all the file names in this directory
	// (i.e. the leaf names not the full path). If dirsAlso is true
	// then also includes sub-directory names.
	void listFileNames(std::set<String>& fileNames, bool dirsAlso = false) const;

private:
	// Constructor which performs no checking on the path which must
	// be a valid absolute path.
	// The bool argument is to discriminate from the other constructor above.
	friend class FileSystemPlatform;
	DirPath(bool, const String& path);

private:

	// The absolute directory path (or "" if invalid)
	String absDirPath_;
};

// An absolute path to a filesystem file.
// It is not necessary for the path to exist.
class FilePath {
public:
	// Construct an invalid file path.
	FilePath();

	// Construct a file path from an absolute file path. The path
	// need not exist. If the path is invalid or relative then this
	// object is marked as invalid. 
	explicit FilePath(const String& absFilePath);

	// Construct a file path from an absolute or relative file
	// path. If the file path is relative then the base directory
	// is used. None of the paths need exist. If the file path is invalid
	// or if the file path is relative and the base directory is invalid
	// then this object is marked as invalid. 
	FilePath(const String& filePath, const DirPath& baseDir);

	// Returns the file path as a string. This will be an absolute path
	// without any . or .. or other special characters and in a format
	// compatible with the native file system. Returns "" if the path is invalid.
	String str() const;

	// Test whether the path is valid - i.e. whether it was marked as valid
	// during construction.
	bool isValid() const;

	// Test whether the path is valid AND exists AND is a file.
	bool exists() const;

	// Get the parent directory of the file or DirPath()
	// if there is an error.
	DirPath getParentDir() const;

	// Get the final directory in the path i.e. a name
	// without any / or \ separators. Returns "" if there is
	// an error.
	//Does not work with Unicode yet - see FileTest.cpp. But
	// since this call is only used for testing not a priority.
	String getLeafName() const;

	// Returns true if this file path is for a virtual file. If virtual
	// file support is enabled then leaf filenames beginning with $ are virtual.
	bool isVirtual() const;

	// Comparison
	bool operator==(const FilePath& other) const;
	bool operator!=(const FilePath& other) const;

private:
	// Constructor which performs no checking on the path.
	// The bool argument is to discriminate from the other constructor above.
	friend class FileSystemPlatform;
	FilePath(bool, const String& path);

private:
	// The absolute file path (or "" if invalid)
	String absFilePath_;
};

// A file system error handler. Users of a file system should install a
// suitable handler by calling "set" and then can retrieve this handler
// globally by calling "get". There is an underlying singleton object.
// If no error handler is set, or if it is set to null then a default
// error handler ignores all file system errors.
class FileSystemErrorHandler {
public:
	// Set the file system error handler. Calling with a 0 argument
	// sets the default error handler which ignores all errors.
	static void set(FileSystemErrorHandlerPtr errorHandler);

	// Get the file system error handler. Always returns a valid
	// error handler.
	static FileSystemErrorHandlerPtr get();

	// Constructor
	FileSystemErrorHandler();

	// Destructor
	virtual ~FileSystemErrorHandler();

	// An input byte sequence cannot be converted to a character.
	// "line" is the line where the character occurred and
	// hexChar is the input sequence in the form "0xab 0xcd ...".
	virtual void cannotEncodeForInput(Line line, const std::string& hexSequence) = 0;

	// An output character cannot be encoded in the output encoding.
	virtual void cannotEncodeForOutput(Char ch) = 0;

	// Unable to open a file for reading
	virtual void cannotOpenForRead(const FilePath& path) = 0;

	// Error during a file read
	virtual void readError(const FilePath& path) = 0;

	// Unable to open a file for writing
	virtual void cannotOpenForWrite(const FilePath& path) = 0;

	// Error during a file write
	virtual void writeError(const FilePath& path) = 0;

private:
	// The current error handler singleton instance
	static FileSystemErrorHandlerPtr& instance();
};

// An encoded file open for input.
// The input is converted to UTF32 according to the input character encoding
// and then any line feed endings are converted to '\n'.
class FileEncodedInput {
private:
	// Constructor.
	// charEncoding is ignored if the file begins with a Unicode BOM. Otherwise
	// it determines the encoding of the file.
	// An error is reported:
	// (1) If the file could not be opened
	// (2) If there is an IO error reading the file
	// (3) If the file has a character which is illegal
	// In the first 2 cases the file is closed and all future reads will return EOFs.
	// For (3) the illegal character will be replaced with a space character.
	FileEncodedInput(const CharEncoding& charEncoding, const FilePath& absFilePath);
	ALLOW_MAKE_SHARED(FileEncodedInput);

public:
	// Create an object
	static FileEncodedInputPtr open(const CharEncoding& charEncoding, const FilePath& absFilePath);

	// Destructor.
	~FileEncodedInput();
	
	// Get the current line number i.e. the number of newlines which have been
	// read + 1. So call read() first and then getLine() to get the line number
	// for the character just read.
	Line getLine() const;

	// Read a single UTF32 character.
	// Line endings are converted to '\n'.
	// Returns EOF at the end of the file.
	Char read();

	// Convenience method.
	// Read up to "size" characters and append to the supplied string.
	// Stops reading if the end of file is reached.
	// Returns the number of bytes read (possibly 0).
	Uint read(String& dst, Uint size = std::numeric_limits<Uint>::max());

	// Convenience method.
	// Read up to "size" characters and return as a string.
	// Stops reading if the end of file is reached.
	String readString(Uint size = std::numeric_limits<Uint>::max());

	// Get the actual character encoding of the file. This may be different
	// from the encoding supplied at creation if the file has a BOM.
	CharEncoding getCharEncoding() const;

private:
	CharEncoding charEncoding_;			// Character encoding
	FilePath absFilePath_;				// File path
	FileRawInputPtr in_;				// Raw input
	CharInputConverterPtr converter_;	// Converter
	Line line_;							// Current line number
	Char newLineChar_;					// Set to last out character if it starts a new line
										// and has been replaced with \n. \0 otherwise.
	static const Uint BUF_SIZE = 1024;	// Internal buffer size
	char buf_[BUF_SIZE];				// Internal buffer
	Uint size_;							// Size of data in internal buffer. If 0 at EOF.
	Uint pos_;							// Position in buffer
}; 

// A binary file open for input.
// There is no conversion of the input data.
class FileBinaryInput {
private:
	// Constructor.
	FileBinaryInput(const FilePath& absFilePath);
	ALLOW_MAKE_SHARED(FileBinaryInput);

public:
	// Create an object
	static FileBinaryInputPtr open(const FilePath& absFilePath);

	// Destructor.
	~FileBinaryInput();

	// Read up to "size" bytes into the supplied buffer.
	// Stops reading if the end of file is reached.
	// Returns the number of bytes read (possibly 0).
	Uint read(char* dst, Uint size);

	// Convenience method.
	// Read up to "size" bytes and append to a std::string.
	void read(std::string& dst, Uint size = std::numeric_limits<Uint>::max());

	// Convenience method.
	// Read up to "size" bytes and return as a std::string.
	std::string readString(Uint size = std::numeric_limits<Uint>::max());

private:
	FilePath absFilePath_;
	FileRawInputPtr in_;
};

// A encoded file open for output.
// First any line feed characters ('\n') are expanded to the new line sequence
// for the platform and then the internal UTF32 is converted according to the
// output character encoding. Characters which cannot be encoded in the output
// encoding are skipped and an error is output.
class FileEncodedOutput  : public OutputStream {
private:
	// Constructor.
	FileEncodedOutput(const CharEncoding& charEncoding, const FilePath& absFilePath);
	ALLOW_MAKE_SHARED(FileEncodedOutput);

public:
	// Create an object
	static FileEncodedOutputPtr create(const CharEncoding& charEncoding, const FilePath& absFilePath);

	// Destructor. Closes the file if it is not closed already.
	~FileEncodedOutput();
	
	// OutputStream virtual method
	void streamChar(Char ch) { write(ch); }

	// Write the supplied character.
	void write(Char src);

	// Convenience method.
	// Write the supplied string.
	void write(const String& src);

	// Close the output file
	void close(); 

	// Returns true if one or more errors has been reported.
	// These can be either IO errors or encoding errors.
	bool hasReportedErrors() const;

private:
	CharEncoding charEncoding_;
	FilePath absFilePath_;
	FileRawOutputPtr out_;
	CharOutputConverterPtr converter_;
	bool encodingErrorReported_;
};

// A binary file open for output.
// There is no conversion of the output data.
class FileBinaryOutput {
private:
	// Constructor.
	// Throws a private exception on failure.
	FileBinaryOutput(const FilePath& absFilePath);
	ALLOW_MAKE_SHARED(FileBinaryOutput);

public:
	// Create an object
	static FileBinaryOutputPtr create(const FilePath& absFilePath);

	// Destructor. Closes the file if it is not closed already.
	~FileBinaryOutput();

	// Write "size" bytes from the supplied buffer.
	void write(const char* src, Uint size);

	// Convenience method.
	// Write "size" bytes from the supplied std::string.
	void write(const std::string& src);

	// Close the output file
	void close(); 

private:
	FilePath absFilePath_;
	FileRawOutputPtr out_;
};

namespace FileSystem {
	// Get the current working directory.
	// Returns DirPath() if there is an error.
	DirPath getWorkingDir();

	// Start or restart the virtual file system. All existing virtual content
	// is deleted.
	//
	// A virtual file has a filename which begins with $. It behaves identically
	// to a normal file except it is held in memory and not written or read from
	// disk. All its parent directories are managed normally and so must exist
	// in the platform file system.
	void startVirtualFileSystem();

	// Stop the virtual file system. All existing virtual content is deleted
	// and files beginning with $ are handled just like any others.
	void stopVirtualFileSystem();

	// Get the separator used between path elements i.e. \ in Windows or / in Linux.
	const String& getPathSeparator();

	// Get the separator used between path elements escaped so that it is in the
	// correct form for appearing within a C++ literal. i.e. \\ in Windows or / in Linux.
	const String& getEscapedPathSeparator();
}
