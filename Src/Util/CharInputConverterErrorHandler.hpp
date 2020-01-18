#pragma once
#include <memory>
#include <string>
#include "Util/Char.hpp"
#include "Util/CharEncoding.hpp"
#include "Util/String.hpp"

DPTR(CharInputConverterErrorHandler)

// An error handler for errors which occur during an input conversion.
// This is not reponsible for recovery from the error - it's only
// purpose is error reporting should that be necessary.
class CharInputConverterErrorHandler {
public:
	virtual ~CharInputConverterErrorHandler() { }

	// An input byte sequence cannot be converted to a character.
	// hexChar is the input sequence in the form "0xab 0xcd ...".
	virtual void cannotEncodeForInput(const std::string& hexChar) = 0;
};
