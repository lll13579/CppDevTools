#include "MakeGen/Dir.hpp"
#include "MakeGen/File.hpp"

///////////////////////////////////////////////////////////
// File
///////////////////////////////////////////////////////////

File::File(const Dir& dir, const std::string& name) :
	dir_(dir),
	stem_(),
	ext_()
{
	std::string::size_type pos = name.find('.');
	ASSERT(pos != std::string::npos);
	stem_ = name.substr(0, pos);
	ext_ = name.substr(pos + 1);
	checkFileStem(stem_);
	checkFileExt(ext_);
}

File::File(const Dir& dir, const std::string& stem, const std::string& ext) :
	dir_(dir),
	stem_(stem),
	ext_(ext)
{
	checkFileStem(stem_);
	checkFileExt(ext_);
}

Dir File::dir() const {
	return dir_;
}

std::string File::name() const {
	return stem_ + '.' + ext_;
}

std::string File::stem() const {
	return stem_;
}

std::string File::ext() const {
	return ext_;
}

bool File::operator<(const File& other) const {
	// Order does not matter so dir_ last since
	// we only have operator< for it.
	if (stem_ < other.stem_) {
		return true;
	}
	else if (stem_ > other.stem_) {
		return false;
	}
	else if (ext_ < other.ext_) {
		return true;
	}
	else if (ext_ > other.ext_) {
		return false;
	}
	else {
		return (dir_ < other.dir_);
	}
}

bool File::exists() const {
	fs::path p = getFsPath();
	return fs::is_regular_file(p) && fs::exists(p);
}

fs::file_time_type File::lastWriteTime() const {
	fs::path p = getFsPath();
	if (!fs::is_regular_file(p) || !fs::exists(p)) {
		return fs::file_time_type();
	}
	else {
		try {
			return fs::last_write_time(p);
		}
		catch (...) {
			return fs::file_time_type();
		}
	}
}

void File::rename(const File& newFile) const {
	ASSERT(dir_.baseIsBuild() || dir_.baseIsTest());
	ASSERT(newFile.dir_.baseIsBuild() || newFile.dir_.baseIsTest());
	fs::path p = getFsPath();
	ASSERT(fs::is_regular_file(p));
	ASSERT(fs::exists(p));
	try {
		return fs::rename(p, newFile.getFsPath());
	}
	catch (...) {
		FAIL;
	}
}

std::string File::str() const {
	char slash = '\\';

	std::string ret = dir_.str();
	ret += slash;
	ret += stem_;
	ret += '.';
	ret += ext_;
	return ret;
}

fs::path File::getFsPath() const {
	fs::path ret = dir_.getFsPath();
	ret /= name();
	return ret;
}

void File::checkFileStem(const std::string& stem) {
	ASSERT(!stem.empty());
	for (std::string::size_type i = 0; i < stem.size(); ++i) {
		char ch = stem[i];
		ASSERT(isalnum((int)ch) || (ch == '_') || (ch == '-'));
	}
}

void File::checkFileExt(const std::string& ext) {
	ASSERT(!ext.empty());
	for (std::string::size_type i = 0; i < ext.size(); ++i) {
		char ch = ext[i];
		ASSERT(isalnum((int)ch) || (ch == '_') || (ch == '-') || (ch == '.'));
	}
	ASSERT(ext[0] != '.');
	ASSERT(ext[ext.size() - 1] != '.');
}

///////////////////////////////////////////////////////////
// os << File
///////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& os, const File& file) {
	os << file.str();
	return os;
}

///////////////////////////////////////////////////////////
// FileList
///////////////////////////////////////////////////////////

FileList::FileList() :
	files_()
{
}

FileList::FileList(std::initializer_list<File> files) :
	files_()
{
	files_.insert(files.begin(), files.end());
}

bool FileList::empty() const {
	return files_.empty();
}

void FileList::add(const File& file) {
	files_.insert(file);
}

void FileList::add(const FileList& fileList) {
	files_.insert(fileList.begin(), fileList.end());
}

void FileList::remove(const File& file) {
	files_.erase(file);
}

void FileList::remove(const FileList& fileList) {
	files_.erase(fileList.begin(), fileList.end());
}

void FileList::remove(const Dir& dir) {
	for (std::set<File>::iterator it = files_.begin(); it != files_.end(); ) {
		if (it->dir() == dir) {
			it = files_.erase(it);
		}
		else {
			++it;
		}
	}
}

void FileList::removeUnlessExt(const std::string& ext) {
	for (std::set<File>::iterator it = files_.begin(); it != files_.end(); ) {
		if (it->ext() != ext) {
			it = files_.erase(it);
		}
		else {
			++it;
		}
	}
}

FileList::const_iterator FileList::begin() const {
	return files_.begin();
}

FileList::const_iterator FileList::end() const {
	return files_.end();
}

///////////////////////////////////////////////////////////
// os << FileList
///////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& os, const FileList& fileList) {
	bool first = true;
	for (const auto& f : fileList) {
		if (first) {
			first = false;
		}
		else {
			os << ' ';
		}
		os << f;
	}
	return os;
}
