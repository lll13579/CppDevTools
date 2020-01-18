#include <iostream>
#include "TestTool/TestFile.hpp"
#include "TestTool/TestFileSystemErrorHandler.hpp"
#include "TestTool/TestUtil.hpp"
#include "Util/Def.hpp"
#include "Util/File.hpp"
#include "Util/SystemCout.hpp"

int main(int argc, const char** argv) {
	scout << "Running TestToolTest" << sendl;
	scout << sendl;

	// Set up the file system error handler
	FileSystemErrorHandler::set(TestFileSystemErrorHandler::instance());

	// Run the tests
	bool success = TestUtil::runTests("TestToolTest", argc, argv);

	return success ? 0 : 1;
}

[[noreturn]] void doAssertFail(std::uint32_t fileLineHash) {
	TestUtil::testAssertionFailure(fileLineHash);
}
