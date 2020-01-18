#pragma once
#include <map>
#include <memory>
#include "Util/Def.hpp"
#include "Util/File.hpp"
#include "Util/String.hpp"

DPTR(FileSystemVirtual)
DPTR(FileVirtualContent)

// The virtual file system maps the name of a virtual file to its
// content. Only absolute file names are allowed (no . or ..).
class FileSystemVirtual {
private:
	ALLOW_MAKE_SHARED(FileSystemVirtual);
	// Create empty file system
	FileSystemVirtual();

public:
	// Singleton instance
	static FileSystemVirtualPtr instance();

	// Enable or disable the virtual file system.
	// Either call has the effect of clearing all existing content
	// although the content of existing open files will persist
	// until they are closed.
	void setEnable(bool status);

	// Returns true if the virtual file system is enabled.
	bool isEnabled();

	// Returns true if the given file path is valid, exists and is a virtual file.
	bool fileExists(const FilePath& path);

	// Opens a virtual file for raw binary input.
	FileRawInputPtr openForInput(const FilePath& path);

	// Opens a virtual file for raw binary output.
	FileRawOutputPtr openForOutput(const FilePath& path);

private:
	// True if the virtual file system is enabled.
	bool isEnabled_;

	// Map from file path to its virtual data.
	typedef std::map<String, FileVirtualContentPtr> MapType;
	MapType map_;
};

