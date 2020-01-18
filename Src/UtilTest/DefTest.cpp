#include <climits>
#include "Util/Def.hpp"
#include "TestTool/TestUtil.hpp"

AUTO_TEST_CASE {
	CHECK(BUILD(WINDOWS) == 1 - BUILD(LINUX));
	CHECK(BUILD(MSV) == 1 - BUILD(GNU));
	CHECK(BUILD(32_BIT) == 1 - BUILD(64_BIT));
	CHECK(BUILD(DEBUG) == 1 - BUILD(RELEASE));
	CHECK((sizeof(Int) == 4) || (sizeof(Int) == 8));
	CHECK((sizeof(Uint) == 4) || (sizeof(Uint) == 8));
	CHECK(sizeof(Uint8) == 1);
	CHECK(sizeof(Uint16) == 2);
	CHECK(sizeof(Uint32) == 4);
	CHECK(sizeof(Uint64) == 8);
	CHECK(sizeof(Int8) == 1);
	CHECK(sizeof(Int16) == 2);
	CHECK(sizeof(Int32) == 4);
	CHECK(sizeof(Int64) == 8);
	CHECK(sizeof(Float32) == 4);
	CHECK(sizeof(Float64) == 8);
	CHECK(sizeof(Char) == 4);
}
