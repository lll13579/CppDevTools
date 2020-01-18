#pragma once
#include <algorithm>
#include "Util/Assert.hpp"
#include "Util/Char.hpp"
#include "Util/CharInputConverter.hpp"
#include "Util/Def.hpp"

// A character input converter from the UTF16BE and UTF16LE character encodings.
class Utf16CharInputConverter : public CharInputConverter {
public:
	Utf16CharInputConverter(bool isBigEndian, CharInputConverterErrorHandlerPtr errorHandler, Char errorChar) :
		CharInputConverter((isBigEndian ? CharEncoding::UTF16BE : CharEncoding::UTF16LE), errorHandler, errorChar),
		reverse_(isBigEndian != BUILD(BIG_ENDIAN))
	{
	}

	std::pair<Uint, Char> convertChar(const char* src, Uint srcLen) {
		ASSERT(srcLen > 0u);

		union {
			char c[4];
			char16_t w[2];
			Uint32 u;
		};
		u = 0;
		memcpy(c, src, std::min(srcLen, 4u));
		if (reverse_) {
			std::swap(c[0], c[1]);
			std::swap(c[2], c[3]);
		}
		Uint len = 0;
		Char ch = Char::fromUtf16(w, len);
		ASSERT(len >= 1u);
		ASSERT(len <= 2u);
		
		if (len * 2u > srcLen) {
			// We have used one of the '\0' characters
			reportError(src, srcLen);
			return std::make_pair(srcLen, errorChar_);
		}

		if (ch.isEof()) {
			reportError(src, len * 2u);
			return std::make_pair(len * 2u, errorChar_);
		}

		return std::make_pair(len * 2u, ch);
	}

private:
	bool reverse_;
};
