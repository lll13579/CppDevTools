#include <iostream>
#include "TestTool/TestFile.hpp"
#include "TestTool/TestFileSystemErrorHandler.hpp"
#include "TestTool/TestUtil.hpp"
#include "Util/Def.hpp"
#include "Util/File.hpp"
#include "Util/SystemCout.hpp"

int main(int argc, const char** argv) {
	scout << "Running UtilTest" << sendl;
	scout << sendl;

	// Set up the file system error handler
	FileSystemErrorHandler::set(TestFileSystemErrorHandler::instance());

	// Run the tests without customisation
	bool success = TestUtil::runTests("UtilTest", argc, argv);

	return success ? 0 : 1;
}

[[noreturn]] void doAssertFail(std::uint32_t fileLineHash) {
	TestUtil::testAssertionFailure(fileLineHash);
}
