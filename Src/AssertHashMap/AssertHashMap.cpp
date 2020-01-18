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

// Find all the assertion lines in a file.
void findAssertionLines(FilePath fpath, std::vector<int>& lines) {
	FileEncodedInputPtr fin = FileEncodedInput::open(CharEncoding::DEFAULT, fpath);
	for (;;) {
		Char ch = fin->read();
		if (ch.isEof()) break;
		if (!ch.isLetter() && (ch != '_')) continue;

		String s;
		do {
			s += ch;
			ch = fin->read();
		} while (ch.isLetter() || (ch == '_'));

		if (((ch == '(') && (s == "ASSERT")) ||
			((ch == ';') && (s == "FAIL"))) 
		{
			lines.push_back(fin->getLine());
		}
	}
}

struct Result {
	Result(std::uint32_t hash_, String file_, int line_) :
		hash(hash_), file(file_), line(line_)
	{
	}
	bool operator<(const Result& other) const {
		if (hash < other.hash) return true;
		if (hash > other.hash) return false;
		if (file < other.file) return true;
		if (file > other.file) return false;
		if (line < other.line) return true;
		return false;
	}
	std::uint32_t hash;
	String file;
	int line;
};

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
	String basePrefix = srcDir.str() + FileSystem::getPathSeparator(); 

	FileSystemErrorHandler::set(LocalFileSystemErrorHandler::instance());

	// Get the file list
	std::vector<FilePath> cppFileList;
	findCppFiles(cppFileList, srcDir);

	// Get the results for each file
	std::set<Result> results;

	for (auto fp : cppFileList) {
		std::vector<int> lines;
		findAssertionLines(fp, lines);
		for (auto line : lines) {
			String file = fp.str();

			// Strip off the prefix up to and including the Src directory.
			StringIterPair pr = file.findFirst(basePrefix);
			ASSERT(pr.first().atBegin());
			String file2 = pr.second().substrAfter();

			std::uint32_t hash = AssertHash::fileLineHash(file2.toUtf8().c_str(), line);
			Result r(hash, file2, line);
			results.insert(r);
		}
	}

	// Output to a string
	std::uint32_t lastHash = 0;
	int collisionCount = 0;
	int count = 0;
	String str;
	for (auto r : results) {
		if (r.hash == lastHash) {
			scout << "Hash collision for hash " << r.hash << sendl;
			++collisionCount;
		}
		else {
			lastHash = r.hash;
		}

		// Hack to fill with zeroes - high
		std::uint32_t nhi = 10000 + (r.hash / 1000000);
		String shi1;
		shi1 << nhi;
		StringIter ithi = ++shi1.begin();
		String shi2 = ithi.substrAfter();

		// Hack to fill with zeroes -low
		std::uint32_t nlo = 1000000 + (r.hash % 1000000);
		String slo1;
		slo1 << nlo;
		StringIter itlo = ++slo1.begin();
		String slo2 = itlo.substrAfter();

		// Output
		str << shi2 
			<< "-" << slo2 
			<< " " << r.file
			<< "(" << r.line << ")" << sendl;
		++count;
	}

	// Output string to a file
	TestFile::createEncodedTestFile(
		CharEncoding::DEFAULT, 
		"AssertHashMap/_HashMap.txt",
		str);

	scout << collisionCount << " hash collisions" << sendl;
	scout << count << " assertion errors" << sendl;
}

// A utility program to examine all source files for ASSERT or
// FAIL macros and then output a map from the file/line hash
// for this assertion error to the file and line number.
// The map is output to Test/.../AssertHashMap/_HashMap.txt.
int main(int argc, char** argv) {
	try {
		if (argc != 1) {
			throw String("no arguments expected for AssertHashMap.exe");
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

