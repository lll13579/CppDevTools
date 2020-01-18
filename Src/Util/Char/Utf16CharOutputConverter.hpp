#pragma once
#include <deque>
#include "Util/Assert.hpp"
#include "Util/CharOutputConverter.hpp"
#include "Util/Def.hpp"

// A character output converter to the UTF16BE and UTF16LE character encodings.
class Utf16CharOutputConverter : public CharOutputConverter {
public:
	Utf16CharOutputConverter(bool isBigEndian) :
		CharOutputConverter(isBigEndian ? CharEncoding::UTF16BE : CharEncoding::UTF16LE),
		reverse_(isBigEndian != BUILD(BIG_ENDIAN))
	{
	}

	~Utf16CharOutputConverter() {
	}

	Uint convertChar(Char ch, char* dst) {
		union {
			char c[4];
			char16_t w[2];
			Uint32 u;
		};
		u = 0;
		Uint ret = 2 * ch.toUtf16(w);
		if (reverse_) {
			std::swap(c[0], c[1]);
			std::swap(c[2], c[3]);
		}
		memcpy(dst, c, ret);
		return ret;
	}

private:
	bool reverse_;
};

