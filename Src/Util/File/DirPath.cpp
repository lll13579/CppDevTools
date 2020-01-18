#include "Util/File.hpp"
#include "Util/File/FileSystemPlatform.hpp"

DirPath::DirPath() :
	absDirPath_() 
{
}
	
DirPath::DirPath(const String& absDirPath) :
	absDirPath_(FileSystemPlatform::instance()->makePath(absDirPath)) 
{
}
	
DirPath::DirPath(const String& dirPath, const DirPath& baseDir) :
	absDirPath_(FileSystemPlatform::instance()->makePath(dirPath, baseDir)) 
{
}

String DirPath::str() const {
	return absDirPath_;
}

bool DirPath::isValid() const {
	return !absDirPath_.empty();
}

bool DirPath::create() const {
	return FileSystemPlatform::instance()->dirCreate(*this);
}

bool DirPath::exists() const {
	return FileSystemPlatform::instance()->dirExists(*this);
}

DirPath DirPath::getParentDir() const {
	return FileSystemPlatform::instance()->parentDir(absDirPath_);
}

String DirPath::getLeafName() const {
	return FileSystemPlatform::instance()->leafName(absDirPath_);
}

bool DirPath::operator==(const DirPath& other) const {
	return absDirPath_ == other.absDirPath_;
}

bool DirPath::operator!=(const DirPath& other) const {
	return absDirPath_ != other.absDirPath_;
}

void DirPath::listFileNames(std::set<String>& fileNames, bool dirsAlso) const {
	FileSystemPlatform::instance()->listFileNames(*this, fileNames, dirsAlso);
}

DirPath::DirPath(bool, const String& path) :
	absDirPath_(path) 
{
}

