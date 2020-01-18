#include "TestTool/Impl/TestCase.hpp"
#include "TestTool/Impl/TestManager.hpp"

namespace TestUtil {

	template <typename X, typename Y>
	void testFailure(const char* file, int line, const char* xExpr, const char* yExpr, X x, Y y, bool isFatal) {
		TestManager::instance().getTestCase()->testFailure(file, line, xExpr, yExpr, x, y, isFatal);
	}

}
