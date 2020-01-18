#include "Util/Assert.hpp"
#include "Util/Char.hpp"
#include "Util/File.hpp"
#include "Util/File/FileRawOutput.hpp"
#include "Util/File/FileSystemPlatform.hpp"
#include "Util/File/FileSystemVirtual.hpp"

FileBinaryOutput::FileBinaryOutput(const FilePath& absFilePath) :
	absFilePath_(absFilePath),
	out_()
{
	out_ = absFilePath.isVirtual() ? FileSystemVirtual::instance()->openForOutput(absFilePath)
								   : FileSystemPlatform::instance()->openForOutput(absFilePath);
	if (out_->failed()) {
		FileSystemErrorHandler::get()->cannotOpenForWrite(absFilePath_);
	}
}

FileBinaryOutputPtr FileBinaryOutput::create(const FilePath& absFilePath) {
	return std::make_shared<FileBinaryOutput>(absFilePath);
}

FileBinaryOutput::~FileBinaryOutput() {
	try {
		close();
	}
	catch (...) {
	}
}

void FileBinaryOutput::write(const char* src, Uint size) {
	if (out_->failed() || (size == 0)) {
		return;
	}
	out_->write(src, size);
	if (out_->failed()) {
		FileSystemErrorHandler::get()->writeError(absFilePath_);
	}
}

void FileBinaryOutput::write(const std::string& src) {
	for (Uint pos = 0; pos < src.size(); pos++) {
		char ch = src[pos];
		write(&ch, 1);
	}
}

void FileBinaryOutput::close() {
	if (!out_->failed()) {
		out_->close();
		if (out_->failed()) {
			FileSystemErrorHandler::get()->writeError(absFilePath_);
		}
	}
}

