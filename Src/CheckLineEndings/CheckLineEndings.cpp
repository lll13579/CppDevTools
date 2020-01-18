#include <set>
#include <vector>
#include "TestTool/TestFile.hpp"
#include "Util/Assert.hpp"
#include "Util/SystemCout.hpp"

// Find all the C++ source and header files in the given directory
// and recursively all its subdirectories.
void findCppFiles(std::vector<FilePath>& cppFileList, DirPath dir) {
	std::set<String> contents;
	dir.listFileNames(contents, true);
	for (std::set<String>::const_iterator it = contents.begin(); it != contents.end(); ++it) {
		FilePath fp(*it, dir);
		if (fp.exists()) {
			if (it->endsWith(".cpp") || it->endsWith(".hpp")) {
				cppFileList.push_back(fp);
			}
		}
		else {
			DirPath dp(*it, dir);
			if (!dp.exists()) {
				throw String("Error: directory member is neither file or directory");
			}
			findCppFiles(cppFileList, dp);
		}
	}
}
// Output a line ending failure message
void badLineEnding(const FilePath& fpath, int line) {
	std::cout << "Bad line ending in " << fpath.str().toUtf8() << " at line " << line << std::endl;
}

// Check all the line endings in a file.
void checkLineEndings(FilePath fpath) {
	FileEncodedInputPtr fin = FileEncodedInput::open(CharEncoding::DEFAULT, fpath);
	if (fin->getCharEncoding() != CharEncoding::DEFAULT) {
		std::cout << "File " << fpath.str().toUtf8() << " has a file encoding of " 
			<< fin->getCharEncoding().getName().toUtf8() << std::endl;
	}
	int line = 1;
	for (;;) {
		// Linux line endings
		Char ch = fin->read();
		if (ch.isEof()) {
			break;
		}
		if (ch == '\n') {
			++line;
		}
		if (ch == '\r') {
			badLineEnding(fpath, line);
		}

#ifdef WINDOWS_LINE_ENDINGS
		if (ch == '\n') {
			badLineEnding(fpath, line);
		}
		if (ch == '\r') {
			ch = fin->read();
			if (ch != '\n') {
				badLineEnding(fpath, line);
			}
			++line;
		}
#endif
	}
}

// An error handler which throws on all errors
class LocalFileSystemErrorHandler : public FileSystemErrorHandler {
public:
	static FileSystemErrorHandlerPtr instance() {
		static FileSystemErrorHandlerPtr ret = std::make_shared<LocalFileSystemErrorHandler>();
		return ret;
	}
	LocalFileSystemErrorHandler() : FileSystemErrorHandler() { }
	~LocalFileSystemErrorHandler() { }
	void cannotEncodeForInput(Line line, const std::string& hexSequence) { throw String("cannot encode file for input"); }
	void cannotEncodeForOutput(Char ch) { throw String("cannot encode file for output"); }
	void cannotOpenForRead(const FilePath& path) { throw String("cannot open file for read"); }
	void readError(const FilePath& path) { throw String("file read error"); }
	void cannotOpenForWrite(const FilePath& path) { throw String("cannot open file for write"); }
	void writeError(const FilePath& path) { throw String("file write error"); }
};

void doMain() {
	DirPath srcDir = TestFile::getSrcDir("");
	if (!srcDir.exists()) {
		throw String("cannot find source directory");
	}

	FileSystemErrorHandler::set(LocalFileSystemErrorHandler::instance());

	// Get the file list
	std::vector<FilePath> cppFileList;
	findCppFiles(cppFileList, srcDir);

	for (auto fp : cppFileList) {
		checkLineEndings(fp);
	}
}

// A utility program to check that all .cpp and .hpp source files have \r\n line endings.
// Also checks for files with an initial BOM.
int main(int argc, char** argv) {
	try {
		if (argc != 1) {
			throw String("no arguments expected for CheckLineEndings.exe");
		}
	
		doMain();
	}
	catch (String& ex) {
		scout << "Error: " << ex << sendl;
		return 1;
	}
	return 0;
}

[[noreturn]] void doAssertFail(std::uint32_t fileLineHash) {
	throw String("assertion error");
}

