#include "TestTool/Impl/TestCase.hpp"
#include "TestTool/Impl/TestManager.hpp"
#include "TestTool/TestUtil.hpp"

namespace TestUtil {

	TestRegistrar::TestRegistrar(const char* file, int line, TestFunc testFunc) {
		TestManager::instance().addTest(new TestCase(file, line, testFunc));
	}

	void expectEvent(TestEventPtr event) {
		TestManager::instance().getTestCase()->expectEvent(event);
	}

	void expectException(const String& what) {
		TestManager::instance().getTestCase()->expectException(what);
	}

	void expectNothing() {
		TestManager::instance().getTestCase()->expectNothing();
	}

	void event(TestEventPtr actualEvent) {
		TestManager::instance().getTestCase()->event(actualEvent);
	}

	void testAssertionFailure(std::uint32_t fileLineHash) {
		TestManager::instance().getTestCase()->testAssertionFailure(fileLineHash);
	}

	void testFailure(const char* file, int line, const char* condition, bool isFatal) {
		TestManager::instance().getTestCase()->testFailure(file, line, condition, isFatal);
	}
	
	Uint getTestFailureCount() {
		return TestManager::instance().getTestCase()->getFailureCount();
	}

	bool runTests(const char* projectName, int argc, const char** argv, TestCustomisePtr customise) {
		return TestManager::instance().runTests(projectName, argc, argv, customise);
	}

}
