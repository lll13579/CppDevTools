#include "Util/Assert.hpp"
#include "Util/Char.hpp"
#include "Util/CharOutputConverter.hpp"
#include "Util/File.hpp"
#include "Util/File/FileRawOutput.hpp"
#include "Util/File/FileSystemPlatform.hpp"
#include "Util/File/FileSystemVirtual.hpp"

FileEncodedOutput::FileEncodedOutput(const CharEncoding& charEncoding, const FilePath& absFilePath) :
	charEncoding_(charEncoding),
	absFilePath_(absFilePath),
	out_(),
	converter_(),
	encodingErrorReported_(false)
{
	out_ = absFilePath.isVirtual() ? FileSystemVirtual::instance()->openForOutput(absFilePath)
								   : FileSystemPlatform::instance()->openForOutput(absFilePath);
	if (out_->failed()) {
		FileSystemErrorHandler::get()->cannotOpenForWrite(absFilePath_);
		// No need to set converter_ - writes will not happen once
		// out_ has failed.
	}
	else {
		converter_ = CharOutputConverter::create(charEncoding_);
	}
}

FileEncodedOutputPtr FileEncodedOutput::create(const CharEncoding& charEncoding, const FilePath& absFilePath) {
	return std::make_shared<FileEncodedOutput>(charEncoding, absFilePath);
}

FileEncodedOutput::~FileEncodedOutput() {
	try {
		close();
	}
	catch (...) {
	}
}

void FileEncodedOutput::write(Char src) {
	if (out_->failed()) {
		return;
	}

#if BUILD(WINDOWS)
	if (src == '\n') {
		// Windows uses a \r\n newline combination
		write(Char('\r'));
	}
#endif

	char buf[CharOutputConverter::MAX_OUTPUT_CHAR_BYTES];
	Uint len = converter_->convertChar(src, buf);

	if (len == 0) {
		encodingErrorReported_ = true;
		FileSystemErrorHandler::get()->cannotEncodeForOutput(src);
	}
	else {
		ASSERT(len <= CharOutputConverter::MAX_OUTPUT_CHAR_BYTES);
		out_->write(buf, len);
		if (out_->failed()) {
			FileSystemErrorHandler::get()->writeError(absFilePath_);
		}
	}
}

void FileEncodedOutput::write(const String& src) {
	for (StringIter pos = src.begin(); !pos.atEnd(); ++pos) {
		write(*pos);
	}
}

void FileEncodedOutput::close() {
	if (!out_->failed()) {
		out_->close();
		if (out_->failed()) {
			FileSystemErrorHandler::get()->writeError(absFilePath_);
		}
	}
}

bool FileEncodedOutput::hasReportedErrors() const {
	return encodingErrorReported_ || out_->failed();
}
