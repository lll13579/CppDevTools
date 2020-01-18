#pragma once
#include <memory>
#include "Util/Def.hpp"

// A raw file input
class FileRawInput {
public:
	// Constructor
	FileRawInput() { }

	// Destructor
	virtual ~FileRawInput() { }
	
	// Read up to "size" bytes into the supplied buffer
	// starting at position pos.
	// Stops reading if the end of file is reached.
	// Returns the number of bytes read (possibly 0).
	virtual Uint read(char* dst, Uint size) = 0;

	// Return true if the raw input has failed for some reason
	// (cannot open file, read error).
	virtual bool failed() = 0;
};
DPTR(FileRawInput)
