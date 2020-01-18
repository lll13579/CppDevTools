#include "Util/File.hpp"
#include "Util/File/FileSystemPlatform.hpp"
#include "Util/File/FileSystemVirtual.hpp"

FilePath::FilePath() :
	absFilePath_() 
{
}
	
FilePath::FilePath(const String& absFilePath) :
	absFilePath_(FileSystemPlatform::instance()->makePath(absFilePath)) 
{
}
	
FilePath::FilePath(const String& filePath, const DirPath& baseDir) :
	absFilePath_(FileSystemPlatform::instance()->makePath(filePath, baseDir)) 
{
}
	
String FilePath::str() const {
	return absFilePath_;
}

bool FilePath::isValid() const {
	return !absFilePath_.empty();
}

bool FilePath::exists() const {
	return isVirtual() ? FileSystemVirtual::instance()->fileExists(*this)
					   : FileSystemPlatform::instance()->fileExists(*this);
}

DirPath FilePath::getParentDir() const {
	return FileSystemPlatform::instance()->parentDir(absFilePath_);
}

String FilePath::getLeafName() const {
	return FileSystemPlatform::instance()->leafName(absFilePath_);
}

bool FilePath::isVirtual() const {
	if (FileSystemVirtual::instance()->isEnabled()) {
		String leaf = getLeafName();
		if (!leaf.empty() && (*leaf.begin() == Char('$'))) {
			return true;
		}
	}
	return false;
}

bool FilePath::operator==(const FilePath& other) const {
	return absFilePath_ == other.absFilePath_;
}

bool FilePath::operator!=(const FilePath& other) const {
	return absFilePath_ != other.absFilePath_;
}

FilePath::FilePath(bool, const String& path) :
	absFilePath_(path) 
{
}

