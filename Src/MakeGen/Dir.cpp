#include "MakeGen/Dir.hpp"
#include "MakeGen/File.hpp"

///////////////////////////////////////////////////////////
// Dir
///////////////////////////////////////////////////////////

Dir::Dir(const Dir& parent, const std::string& subdir) :
	baseType_(parent.baseType_),
	dirNames_(parent.dirNames_)
{
	checkDirName(subdir);
	dirNames_.push_back(subdir);
}

Dir Dir::SRC() {
	static Dir x(BASE_SRC);
	return x;
}

Dir Dir::BUILD() {
	static Dir x(BASE_BUILD);
	return x;
}

Dir Dir::TEST() {
	static Dir x(BASE_TEST);
	return x;
}

Dir Dir::SRC(const std::string& projectName) {
	return Dir(Dir::SRC(), projectName);
}

Dir Dir::BUILD(const std::string& projectName) {
	return Dir(Dir::BUILD(), projectName);
}

Dir Dir::TEST(const std::string& projectName) {
	return Dir(Dir::TEST(), projectName);
}

Dir Dir::operator/(const std::string& subdir) const {
	return Dir(*this, subdir);
}

bool Dir::baseIsSrc() const {
	return (baseType_ == BASE_SRC);
}

bool Dir::baseIsBuild() const {
	return (baseType_ == BASE_BUILD);
}

bool Dir::baseIsTest() const {
	return (baseType_ == BASE_TEST);
}

Dir Dir::parent() const {
	ASSERT(!dirNames_.empty());
	Dir ret = *this;
	ret.dirNames_.pop_back();
	return ret;
}

std::string Dir::name() const {
	ASSERT(!dirNames_.empty());
	return dirNames_.back();
}

DirList Dir::allSubdirs() const {
	ASSERT(baseIsSrc());
	fs::path p = getFsPath();
	ASSERT(fs::is_directory(p));
	DirList ret;
	for (const auto& d : fs::directory_iterator(p)) {
		if (fs::is_directory(d)) {
			std::string name = d.path().filename().string();
			if (name == ".vs") {
				continue;
			}
			checkDirName(name);
			ret.add(Dir(*this, name));
		}
	}
	return ret;
}

FileList Dir::allFiles() const {
	ASSERT(baseIsSrc());
	fs::path p = getFsPath();
	ASSERT(fs::is_directory(p));
	FileList ret;
	for (const auto& f : fs::directory_iterator(p)) {
		if (fs::is_regular_file(f)) {
			std::string name = f.path().filename().string();
			ret.add(File(*this, name));
		}
	}
	return ret;
}

FileList Dir::allFilesRecursive() const {
	FileList ret;
	DirList inDirs;
	inDirs.add(*this);
	DirList outDirs;
	while (!inDirs.empty()) {
		for (const auto& inDir : inDirs) {
			ret.add(inDir.allFiles());
			outDirs.add(inDir.allSubdirs());
		}
		inDirs = outDirs;
		outDirs = DirList();
	}
	return ret;
}

bool Dir::operator<(const Dir& other) const {
	if (baseType_ < other.baseType_) {
		return true;
	}
	else if (baseType_ > other.baseType_) {
		return false;
	}
	else {
		return (dirNames_ < other.dirNames_);
	}
}

bool Dir::operator==(const Dir& other) const {
	return (baseType_ == other.baseType_) &&
		   (dirNames_ == other.dirNames_);
}

bool Dir::operator!=(const Dir& other) const {
	return !(*this == other);
}

void Dir::create() {
	ASSERT((baseType_ == BASE_BUILD) || (baseType_ == BASE_TEST));
	fs::path p = getFsPath();
	try {
		fs::create_directories(p);
	}
	catch (...) {
		FAIL;
	}
}

void Dir::removeContents() {
	ASSERT((baseType_ == BASE_BUILD) || (baseType_ == BASE_TEST));
	fs::path p = getFsPath();
	if (fs::exists(p)) {
		try {
			fs::remove_all(p);
		}
		catch (...) {
			FAIL;
		}
	}
	create();
}

std::string Dir::str() const {
	char slash = '\\';

	std::string ret;
	switch (baseType_) {
	case BASE_SRC:
		ret += "Src";
		break;
	case BASE_BUILD:
		ret += "Build";
		ret += slash;
		ret += BUILD_NAME;
		break;
	case BASE_TEST:
		ret += "Test";
		ret += slash;
		ret += BUILD_NAME;
		break;
	default:
		FAIL;
	}

	for (const auto& d : dirNames_) {
		ret += slash;
		ret += d;
	}

	return ret;
}

fs::path Dir::getFsPath() const {
	return getBaseParent() / str();
}

Dir::Dir(BaseType baseType) :
	baseType_(baseType),
	dirNames_()
{
}

void Dir::checkDirName(const std::string& subdir) {
	ASSERT(!subdir.empty());
	for (std::string::size_type i = 0; i < subdir.size(); ++i) {
		char ch = subdir[i];
		ASSERT(isalnum((int)ch) || (ch == '_') || (ch == '-'));
	}
}

fs::path Dir::getBaseParent() {
	static fs::path ret;
	if (ret == fs::path()) {
		// One time initialisation.
		std::string sFile = __FILE__;

		// Unfortunately MSVC makes everything lower case. Convert the
		// start of each name at the start of after \ to uppercase.
		bool nextToUpper = true;
		for (std::string::size_type i = 0; i < sFile.size(); ++i) {
			char ch = sFile[i];
			if (nextToUpper) {
				sFile[i] = (char)toupper(ch);
				nextToUpper = false;
			}
			if (ch == '\\') {
				nextToUpper = true;
			}
		}

		fs::path p(sFile);

		ASSERT(fs::is_regular_file(p));
		ASSERT(p.filename().string() == "Dir.cpp");
		p = p.parent_path();

		ASSERT(fs::is_directory(p));
		ASSERT(p.filename().string() == "Makegen");
		p = p.parent_path();

		ASSERT(fs::is_directory(p));
		ASSERT(p.filename().string() == "Src");
		ret = p.parent_path();
	}
	return ret;
}

///////////////////////////////////////////////////////////
// os << Dir
///////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& os, const Dir& dir) {
	os << dir.str();
	return os;
}

///////////////////////////////////////////////////////////
// DirList
///////////////////////////////////////////////////////////

DirList::DirList() :
	dirs_()
{
}

bool DirList::empty() const {
	return dirs_.empty();
}

void DirList::add(const Dir& dir) {
	dirs_.insert(dir);
}

void DirList::add(const DirList& dirList) {
	dirs_.insert(dirList.begin(), dirList.end());
}

void DirList::remove(const Dir& dir) {
	dirs_.erase(dir);
}

void DirList::remove(const DirList& dirList) {
	dirs_.erase(dirList.begin(), dirList.end());
}

DirList::const_iterator DirList::begin() const {
	return dirs_.begin();
}

DirList::const_iterator DirList::end() const {
	return dirs_.end();
}

///////////////////////////////////////////////////////////
// os << DirList
///////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& os, const DirList& dirList) {
	bool first = true;
	for (const auto& d : dirList) {
		if (first) {
			first = false;
		}
		else {
			os << ' ';
		}
		os << d;
	}
	return os;
}
