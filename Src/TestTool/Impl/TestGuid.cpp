#include <iomanip>
#include <sstream>
#include "Rpc.h"
#include "Util/Assert.hpp"
#pragma comment(lib, "Rpcrt4.lib")

namespace TestGuid {

	std::string createGuid(bool isUpperNotLower) {
		// Create the GUID.  Note that we use the UuidCreate call rather than
		// CoCreateGuid because this creates a GUID which cannot be traced to
		// the network address of the generating computer.
		GUID guid;
		RPC_STATUS status = UuidCreate(&guid);
		ASSERT(status == RPC_S_OK);

		std::ostringstream ss;
		ss << std::hex << std::setfill('0');
		if (isUpperNotLower) {
			ss << std::uppercase;
		}
		ss << std::setw(8) << guid.Data1
			<< "-"
			<< std::setw(4) << guid.Data2
			<< "-"
			<< std::setw(4) << guid.Data3
			<< "-";
		for (int i = 0; i < 8; ++i) {
			if (i == 2) {
				ss << "-";
			}
			ss << std::setw(2) << (unsigned)guid.Data4[i];
		}
		return ss.str();
	}

}
