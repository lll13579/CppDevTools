#include <algorithm>
#include "Util/Assert.hpp"
#include "Util/Char.hpp"
#include "Util/File.hpp"
#include "Util/File/FileRawInput.hpp"
#include "Util/File/FileSystemPlatform.hpp"
#include "Util/File/FileSystemVirtual.hpp"

FileBinaryInput::FileBinaryInput(const FilePath& absFilePath) :
	absFilePath_(absFilePath),
	in_()
{
	in_ = absFilePath.isVirtual() ? FileSystemVirtual::instance()->openForInput(absFilePath)
								  : FileSystemPlatform::instance()->openForInput(absFilePath);
	if (in_->failed()) {
		FileSystemErrorHandler::get()->cannotOpenForRead(absFilePath_);
	}
}

FileBinaryInputPtr FileBinaryInput::open(const FilePath& absFilePath) {
	return std::make_shared<FileBinaryInput>(absFilePath);
}

FileBinaryInput::~FileBinaryInput() {
}

Uint FileBinaryInput::read(char* dst, Uint size) {
	if (in_->failed()) {
		return 0;
	}
	Uint readSize = in_->read(dst, size);
	if (in_->failed()) {
		FileSystemErrorHandler::get()->readError(absFilePath_);
		return 0;
	}

	return readSize;
}

void FileBinaryInput::read(std::string& dst, Uint size) {
	Uint remaining = size;
	const Uint BUF_SIZE = 1024;
	char buf[BUF_SIZE];
	while (remaining > 0) {
		Uint len = read(buf, std::min(remaining, BUF_SIZE));
		if (len == 0) {
			break;
		}
		dst.append(buf, len);
		remaining -= len;
	}
}

std::string FileBinaryInput::readString(Uint size) {
	std::string ret;
	read(ret, size);
	return ret;
}

