#include <iomanip>
#include <sstream>
#include "Util/Assert.hpp"
#include "Util/Char.hpp"
#include "Util/CharInputConverter.hpp"
#include "Util/Def.hpp"
#include "Util/File.hpp"
#include "Util/File/FileRawInput.hpp"
#include "Util/File/FileSystemPlatform.hpp"
#include "Util/File/FileSystemVirtual.hpp"

///////////////////////////////////////////////////////////////////////////////
// InputFileCharInputConverterErrorHandler
///////////////////////////////////////////////////////////////////////////////

// An error handler for errors which occur during conversion for
// an input file.
class InputFileCharInputConverterErrorHandler : public CharInputConverterErrorHandler {
public:
	// Create the error handler.
	static CharInputConverterErrorHandlerPtr create(const Line* line) {
		return std::make_shared<InputFileCharInputConverterErrorHandler>(line);
	}

	~InputFileCharInputConverterErrorHandler() {
	}

	void cannotEncodeForInput(const std::string& hexSequence) {
		FileSystemErrorHandler::get()->cannotEncodeForInput(*line_, hexSequence);
	}
private:
	InputFileCharInputConverterErrorHandler(const Line* line) :
		line_(line)
	{ 
	}
	ALLOW_MAKE_SHARED(InputFileCharInputConverterErrorHandler);
private:
	const Line* line_;
};

///////////////////////////////////////////////////////////////////////////////
// FileEncodedInput
///////////////////////////////////////////////////////////////////////////////

FileEncodedInput::FileEncodedInput(const CharEncoding& charEncoding, const FilePath& absFilePath) :
	charEncoding_(charEncoding),
	absFilePath_(absFilePath),
	in_(),
	converter_(),
	line_(1),
	newLineChar_('\0'),
	// buf_,
	size_(0),
	pos_(0)
{
	ASSERT(charEncoding_.isValid());

	in_ = absFilePath.isVirtual() ? FileSystemVirtual::instance()->openForInput(absFilePath)
								  : FileSystemPlatform::instance()->openForInput(absFilePath);
	if (in_->failed()) {
		FileSystemErrorHandler::get()->cannotOpenForRead(absFilePath_);
		return;
	}

	size_ = in_->read(buf_, BUF_SIZE);
	if (in_->failed()) {
		FileSystemErrorHandler::get()->readError(absFilePath_);
		return;
	}

	// Look for BOM
	if ((size_ >= 3) &&
		(buf_[0] == (char)0xef) &&
		(buf_[1] == (char)0xbb) &&
		(buf_[2] == (char)0xbf))
	{
		charEncoding_ = CharEncoding::UTF8;
		pos_ = 3;
	}
	else if ((size_ >= 4) &&
		(buf_[0] == (char)0x00) &&
		(buf_[1] == (char)0x00) &&
		(buf_[2] == (char)0xfe) &&
		(buf_[3] == (char)0xff))
	{
		charEncoding_ = CharEncoding::UTF32BE;
		pos_ = 4;
	}
	else if ((size_ >= 4) &&
		(buf_[0] == (char)0xff) &&
		(buf_[1] == (char)0xfe) &&
		(buf_[2] == (char)0x00) &&
		(buf_[3] == (char)0x00))
	{
		charEncoding_ = CharEncoding::UTF32LE;
		pos_ = 4;
	}
	else if ((size_ >= 2) &&
		(buf_[0] == (char)0xfe) &&
		(buf_[1] == (char)0xff))
	{
		charEncoding_ = CharEncoding::UTF16BE;
		pos_ = 2;
	}
	else if ((size_ >= 2) &&
		(buf_[0] == (char)0xff) &&
		(buf_[1] == (char)0xfe))
	{
		charEncoding_ = CharEncoding::UTF16LE;
		pos_ = 2;
	}

	converter_ = CharInputConverter::create(
		charEncoding_, 
		InputFileCharInputConverterErrorHandler::create(&line_), 
		Char(' '));
}

FileEncodedInput::~FileEncodedInput() {
}

FileEncodedInputPtr FileEncodedInput::open(const CharEncoding& charEncoding, const FilePath& absFilePath) {
	return std::make_shared<FileEncodedInput>(charEncoding, absFilePath);
}

Line FileEncodedInput::getLine() const {
	return line_;
}

Char FileEncodedInput::read() {
	// Return if already at EOF or if IO has failed.
	if ((size_ == 0) || in_->failed()) {
		return Char::eof();
	}

	// Increment the line number for the character after a newline
	if (newLineChar_ != '\0') {
		line_++;
	}

	if (size_ - pos_ < CharInputConverter::MAX_INPUT_CHAR_BYTES) {
		// We do not have enough data in the converter.
		memmove(buf_, buf_ + pos_, size_ - pos_);
		size_ -= pos_;
		pos_ = 0;

		// Try to read in more data from file
		Uint toRead = BUF_SIZE - size_; 
		Uint readSize = in_->read(buf_ + size_, toRead);
		ASSERT(readSize <= toRead);
		size_ += readSize;

		if (in_->failed()) {		
			FileSystemErrorHandler::get()->readError(absFilePath_);
			return Char::eof();
		}
	}

	if (pos_ == size_) {
		// No more characters
		newLineChar_ == '\0';
		size_ = 0;
		return Char::eof();
	}

	// Convert one character
	std::pair<Uint, Char> out = converter_->convertChar(buf_ + pos_, size_ - pos_);
	ASSERT(out.first > 0);
	ASSERT(pos_ + out.first <= size_);
	pos_ += out.first;

	Char ch = out.second;

	if (newLineChar_ == '\r') {
		// \r\n is a single newline
		newLineChar_ = '\0';
		if (ch == '\n') {
			return read();
		}
	}
	else if (newLineChar_ == '\n') {
		// \n\r is a single newline
		newLineChar_ = '\0';
		if (ch == '\r') {
			return read();
		}
	}

	// Detect the start of a new newline and return \n if so.
	if ((ch == '\n') || (ch == '\r')) {
		newLineChar_ = ch;
		ch = '\n';
	}

	return ch;
}

Uint FileEncodedInput::read(String& dst, Uint size) {
	Uint ret = 0;
	for (ret = 0; ret < size; ret++) {
		Char ch = read();
		if (ch.isEof()) {
			break;
		}
		dst += ch;
	}
	return ret;
}

String FileEncodedInput::readString(Uint size) {
	String ret;
	read(ret, size);
	return ret;
}

CharEncoding FileEncodedInput::getCharEncoding() const {
	return charEncoding_;
}
