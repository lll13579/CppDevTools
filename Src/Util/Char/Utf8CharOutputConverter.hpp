#pragma once
#include "Util/Assert.hpp"
#include "Util/CharOutputConverter.hpp"
#include "Util/Def.hpp"

// A character output converter to the UTF8 character encoding.
class Utf8CharOutputConverter : public CharOutputConverter {
public:
	Utf8CharOutputConverter() :
		CharOutputConverter(CharEncoding::UTF8)
	{
	}

	~Utf8CharOutputConverter() {
	}

	Uint convertChar(Char ch, char* dst) {
		return ch.toUtf8(dst);
	}
};
