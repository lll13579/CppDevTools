#pragma once
#include <memory>
#include "Util/Def.hpp"

// A raw file output
class FileRawOutput {
public:
	// Constructor
	FileRawOutput() { }

	// Destructor
	virtual ~FileRawOutput() { }
	
	// Write "size" bytes from "src" to the end of the file
	virtual void write(const char* src, Uint size) = 0;

	// Close and save the output file
	virtual void close() = 0;

	// Return true if the output has failed
	virtual bool failed() = 0;
};
DPTR(FileRawOutput)
