#pragma once
#include <deque>
#include "Util/Assert.hpp"
#include "Util/CharOutputConverter.hpp"
#include "Util/Def.hpp"

// A character output converter to the UTF32BE and UTF32LE character encoding.
class Utf32CharOutputConverter : public CharOutputConverter {
public:
	Utf32CharOutputConverter(bool isBigEndian) :
		CharOutputConverter(isBigEndian ? CharEncoding::UTF32BE : CharEncoding::UTF32LE),
		reverse_(isBigEndian != BUILD(BIG_ENDIAN))
	{
	}

	~Utf32CharOutputConverter() {
	}

	Uint convertChar(Char ch, char* dst) {
		union {
			char c[4];
			char32_t w;
		};
		w = ch.toUtf32();
		if (reverse_) {
			std::swap(c[0], c[3]);
			std::swap(c[1], c[2]);
		}
		memcpy(dst, c, 4);
		return 4u;
	}

private:
	bool reverse_;
};

