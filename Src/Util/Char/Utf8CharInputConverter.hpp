#pragma once
#include "Util/Assert.hpp"
#include "Util/Char.hpp"
#include "Util/CharInputConverter.hpp"
#include "Util/Def.hpp"

// A character input converter from the UTF8 character encoding.
class Utf8CharInputConverter : public CharInputConverter {
public:
	Utf8CharInputConverter(CharInputConverterErrorHandlerPtr errorHandler, Char errorChar) :
		CharInputConverter(CharEncoding::UTF8, errorHandler, errorChar)
	{
	}

	std::pair<Uint, Char> convertChar(const char* src, Uint srcLen) {
		ASSERT(srcLen > 0);
		const char* psrc = src;
		if (srcLen <= 4u) {
			// Pain but we need a separate buffer
			char buf[5];
			memcpy(buf, src, srcLen);
			buf[srcLen] = '\0';
			psrc = buf;
		}

		Uint len;
		Char ch = Char::fromUtf8(psrc, len);
		ASSERT(len > 0u);
		ASSERT(len <= srcLen);

		if (ch.isEof()) {
			reportError(src, len);
			ch = errorChar_;
		}
		return std::make_pair(len, ch);
	}
};
