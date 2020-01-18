#include <algorithm>
#include <deque>
#include "Util/File.hpp"
#include "Util/File/FileRawInput.hpp"
#include "Util/File/FileRawOutput.hpp"
#include "Util/File/FileSystemVirtual.hpp"
#include "Util/Assert.hpp"

///////////////////////////////////////////////////////////////////////////////
// FileVirtualContent
///////////////////////////////////////////////////////////////////////////////

// The contents of a virtual file (stored in binary)
class FileVirtualContent {
public:
	// Constructor for an empty file
	FileVirtualContent() :
		data_()
	{
	}
	
	// Get the file size in bytes
	Uint getSize() const { 
		return (Uint)data_.size(); 
	}

	// Read up to "size" bytes into the supplied buffer
	// starting at position pos.
	// Stops reading if the end of file is reached.
	// Returns the number of bytes read (possibly 0).
	Uint read(char* dst, Uint pos, Uint size) {
		if (pos >= getSize()) {
			return 0u;
		}
		else {
			Uint actualSize = std::min(size, getSize() - pos);
			char* p = dst;
			std::deque<char>::const_iterator it = data_.begin() + pos;
			for (Uint i = 0; i < actualSize; ++i) {
				*p++ = *it++;
			}
			return actualSize;
		}
	}

	// Write "size" bytes from "src" to the end of the file
	void write(const char* src, Uint size) { 
		const char* p = src;
		for (Uint i = 0; i < size; ++i) {
			data_.push_back(*p++);
		}
	}

private:
	std::deque<char> data_;
};

///////////////////////////////////////////////////////////////////////////////
// VirtualFileRawInput
///////////////////////////////////////////////////////////////////////////////

// The only failure is when no content is supplied at the outset (presumably
// because the file could not be found).
class VirtualFileRawInput : public FileRawInput {
public:
	VirtualFileRawInput(FileVirtualContentPtr content) :
		FileRawInput(),
		content_(content),
		pos_(0)
	{
	}
	~VirtualFileRawInput() {
	}
	Uint read(char* dst, Uint size) {
		if (!content_ || (size == 0)) {
			return 0;
		}

		Uint readSize = content_->read(dst, pos_, size);
		pos_ += readSize;
		return readSize;
	}
	bool failed() {
		return content_ == 0;
	}
private:
	FileVirtualContentPtr content_;
	Uint pos_;
};

///////////////////////////////////////////////////////////////////////////////
// VirtualFileRawOutput
///////////////////////////////////////////////////////////////////////////////

// The only failure is when no content is supplied at the outset (presumably
// because the file could not be created).
class VirtualFileRawOutput : public FileRawOutput {
public:
	VirtualFileRawOutput(FileVirtualContentPtr content) :
		FileRawOutput(),
		content_(content)
	{
	}
	~VirtualFileRawOutput() {
		close();
	}
	void write(const char* src, Uint size) {
		if (!content_ || (size == 0)) {
			return;
		}
		content_->write(src, size);
	}
	void close() {
	}
	bool failed() {
		return content_ == 0;
	}
private:
	FileVirtualContentPtr content_;
};

///////////////////////////////////////////////////////////////////////////////
// FileSystemVirtual
///////////////////////////////////////////////////////////////////////////////

FileSystemVirtual::FileSystemVirtual() :
	isEnabled_(false),
	map_()
{
}

FileSystemVirtualPtr FileSystemVirtual::instance() {
	static FileSystemVirtualPtr ret = std::make_shared<FileSystemVirtual>();
	return ret;
}

void FileSystemVirtual::setEnable(bool status) {
	isEnabled_ = status;
	map_.clear();
}

bool FileSystemVirtual::isEnabled() {
	return isEnabled_;
}

bool FileSystemVirtual::fileExists(const FilePath& path) {
	if (!path.isValid()) {
		return false;
	}
	else {
		MapType::const_iterator it = map_.find(path.str());
		return it != map_.end();
	}
}

FileRawInputPtr FileSystemVirtual::openForInput(const FilePath& path) {
	FileVirtualContentPtr content;

	MapType::const_iterator it = map_.find(path.str());
	if (it != map_.end()) {
		content = it->second;
	}
	return std::make_shared<VirtualFileRawInput>(content);
}

FileRawOutputPtr FileSystemVirtual::openForOutput(const FilePath& path) {
	// Create new content
	FileVirtualContentPtr content = std::make_shared<FileVirtualContent>();

	MapType::iterator it = map_.find(path.str());
	if (it != map_.end()) {
		// Overwrite existing content
		it->second = content;
	}
	else {
		// Create a new entry
		map_.insert(std::make_pair(path.str(), content));
	}

	return std::make_shared<VirtualFileRawOutput>(content);
}
