#pragma once
#include "Util/Assert.hpp"
#include "Util/Char.hpp"
#include "Util/CharInputConverter.hpp"
#include "Util/Def.hpp"

// A character input converter from the UTF32BE and UTF32LE character encoding.
class Utf32CharInputConverter : public CharInputConverter {
public:
	Utf32CharInputConverter(bool isBigEndian, CharInputConverterErrorHandlerPtr errorHandler, Char errorChar) :
		CharInputConverter((isBigEndian ? CharEncoding::UTF32BE : CharEncoding::UTF32LE), errorHandler, errorChar),
		reverse_(isBigEndian != BUILD(BIG_ENDIAN))
	{
	}

	std::pair<Uint, Char> convertChar(const char* src, Uint srcLen) {
		ASSERT(srcLen > 0u);

		if (srcLen < 4u) {
			reportError(src, srcLen);
			return std::make_pair(srcLen, errorChar_);
		}

		union {
			char c[4];
			char32_t w;
		};
		memcpy(c, src, 4u);
		if (reverse_) {
			std::swap(c[0], c[3]);
			std::swap(c[1], c[2]);
		}
		Char ch = Char::fromUtf32(w);

		if (ch.isEof()) {
			reportError(src, 4u);
			return std::make_pair(4u, errorChar_);
		}

		return std::make_pair(4u, ch);
	}

private:
	bool reverse_;
};
