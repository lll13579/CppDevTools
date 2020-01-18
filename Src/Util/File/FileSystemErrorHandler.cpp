#include "Util/Char.hpp"
#include "Util/File.hpp"

///////////////////////////////////////////////////////////////////////////////
// DefaultFileSystemErrorHandler
///////////////////////////////////////////////////////////////////////////////

// A default error handler which ignores all errors
class DefaultFileSystemErrorHandler : public FileSystemErrorHandler {
public:
	static FileSystemErrorHandlerPtr instance() {
		static FileSystemErrorHandlerPtr ret = std::make_shared<DefaultFileSystemErrorHandler>();
		return ret;
	}
	DefaultFileSystemErrorHandler() : FileSystemErrorHandler() { }
	~DefaultFileSystemErrorHandler() { }
	void cannotEncodeForInput(Line line, const std::string& hexSequence) { }
	void cannotEncodeForOutput(Char ch) { }
	void cannotOpenForRead(const FilePath& path) { }
	void readError(const FilePath& path) { }
	void cannotOpenForWrite(const FilePath& path) { }
	void writeError(const FilePath& path) { }
};

///////////////////////////////////////////////////////////////////////////////
// FileSystemErrorHandler
///////////////////////////////////////////////////////////////////////////////

void FileSystemErrorHandler::set(FileSystemErrorHandlerPtr errorHandler) {
	instance() = errorHandler ? errorHandler : DefaultFileSystemErrorHandler::instance();
}

FileSystemErrorHandlerPtr FileSystemErrorHandler::get() {
	return instance();
}

FileSystemErrorHandler::FileSystemErrorHandler() { 
}

FileSystemErrorHandler::~FileSystemErrorHandler() { 
}

FileSystemErrorHandlerPtr& FileSystemErrorHandler::instance() {
	static FileSystemErrorHandlerPtr ret;
	return ret;
}
