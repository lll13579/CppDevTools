#pragma once
#include <string>

namespace TestGuid {

	// Returns a newly created GUID.
	std::string createGuid(bool isUpperNotLower = true);

}
