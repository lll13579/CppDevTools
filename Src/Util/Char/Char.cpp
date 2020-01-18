#include "Util/Assert.hpp"
#include "Util/Char.hpp"
#include "Util/OutputStream.hpp"

namespace {
	// The properties of a source character (i.e. a character in a source file).
	// These characters are UTF32.
	enum {
		// The following categories are mutually exclusive and only apply to the
		// ASCII 7 bit range of the UTF32 spectrum.
		PROPERTY_UPPERCASE			= 0x0001,		// Upper case ASCII letter.
		PROPERTY_LOWERCASE			= 0x0002,		// Lower case ASCII letter.
		PROPERTY_DIGIT				= 0x0004,		// Digit 0-9.
		PROPERTY_UNDERSCORE			= 0x0008,		// Underscore character.
		PROPERTY_WHITESPACE			= 0x0020,		// Whitespace " \t\v\f\n" (not "\r").
		PROPERTY_OTHER_BASIC_SOURCE = 0x0040,		// Other characters in basic source character set
													// excluding the above.
		PROPERTY_BAD_SOURCE			= 0x0080,		// A character not allowed as a source character
		PROPERTY_OTHER_ASCII		= 0x0100,		// Other single ASCII character not listed above
		// Other properties
		PROPERTY_HEX_LETTER			= 0x0200,		// Hex letter from A to F or a to f
	};

#define U PROPERTY_UPPERCASE
#define L PROPERTY_LOWERCASE
#define D PROPERTY_DIGIT
#define _ PROPERTY_UNDERSCORE
#define W PROPERTY_WHITESPACE
#define S PROPERTY_OTHER_BASIC_SOURCE
#define B PROPERTY_BAD_SOURCE
#define O PROPERTY_OTHER_ASCII		// 0x24 $, 0x40 @, 0x60 `
#define H PROPERTY_HEX_LETTER

	const Uint ASCII_MAX = 0x80u;
	const Uint PROPERTIES[ASCII_MAX] =
	{	/*              0		1		2		3		4		5		6		7		8		9		A		B		C		D		E		F	
		/* 00 */		B,		B,		B,		B,		B,		B,		B,		B,		B,		W,		W,		W,		W,		B,		B,		B,
		/* 10 */		B,		B,		B,		B,		B,		B,		B,		B,		B,		B,		B,		B,		B,		B,		B,		B,
		/* 20 */		W,		S,		S,		S,		O,		S,		S,		S,		S,		S,		S,		S,		S,		S,		S,		S,
		/* 30 */		D,		D,		D,		D,		D,		D,		D,		D,		D,		D,		S,		S,		S,		S,		S,		S,
		/* 40 */		O,		U|H,	U|H,	U|H,	U|H,	U|H,	U|H,	U,		U,		U,		U,		U,		U,		U,		U,		U,
		/* 50 */		U,		U,		U,		U,		U,		U,		U,		U,		U,		U,		U,		S,		S,		S,		S,		_,
		/* 60 */		O,		L|H,	L|H,	L|H,	L|H,	L|H,	L|H,	L,		L,		L,		L,		L,		L,		L,		L,		L,
		/* 70 */		L,		L,		L,		L,		L,		L,		L,		L,		L,		L,		L,		S,		S,		S,		S,		B,
	};

	const struct {
		Uint32 first;
		Uint32 last;
	} UTF32_IDENTIFIER_RANGES[] =
	{
		{0x00c0, 0x00d6}, {0x00d8, 0x00f6}, {0x00f8, 0x01f5}, {0x01fa, 0x0217}, {0x0250, 0x02a8}, {0x0384, 0x0384}, {0x0388, 0x038a}, {0x038c, 0x038c},
		{0x038e, 0x03a1}, {0x03a3, 0x03ce}, {0x03d0, 0x03d6}, {0x03da, 0x03da}, {0x03dc, 0x03dc}, {0x03de, 0x03de}, {0x03e0, 0x03e0}, {0x03e2, 0x03f3},
		{0x0401, 0x040d}, {0x040f, 0x044f}, {0x0451, 0x045c}, {0x045e, 0x0481}, {0x0490, 0x04c4}, {0x04c7, 0x04c8}, {0x04cb, 0x04cc}, {0x04d0, 0x04eb},
		{0x04ee, 0x04f5}, {0x04f8, 0x04f9}, {0x0531, 0x0556}, {0x0561, 0x0587}, {0x05d0, 0x05ea}, {0x05f0, 0x05f4}, {0x0621, 0x063a}, {0x0640, 0x0652},
		{0x0670, 0x06b7}, {0x06ba, 0x06be}, {0x06c0, 0x06ce}, {0x06e5, 0x06e7}, {0x0905, 0x0939}, {0x0958, 0x0962}, {0x0985, 0x098c}, {0x098f, 0x0990},
		{0x0993, 0x09a8}, {0x09aa, 0x09b0}, {0x09b2, 0x09b2}, {0x09b6, 0x09b9}, {0x09dc, 0x09dd}, {0x09df, 0x09e1}, {0x09f0, 0x09f1}, {0x0a05, 0x0a0a},
		{0x0a0f, 0x0a10}, {0x0a13, 0x0a28}, {0x0a2a, 0x0a30}, {0x0a32, 0x0a33}, {0x0a35, 0x0a36}, {0x0a38, 0x0a39}, {0x0a59, 0x0a5c}, {0x0a5e, 0x0a5e},
		{0x0a85, 0x0a8b}, {0x0a8d, 0x0a8d}, {0x0a8f, 0x0a91}, {0x0a93, 0x0aa8}, {0x0aaa, 0x0ab0}, {0x0ab2, 0x0ab3}, {0x0ab5, 0x0ab9}, {0x0ae0, 0x0ae0},
		{0x0b05, 0x0b0c}, {0x0b0f, 0x0b10}, {0x0b13, 0x0b28}, {0x0b2a, 0x0b30}, {0x0b32, 0x0b33}, {0x0b36, 0x0b39}, {0x0b5c, 0x0b5d}, {0x0b5f, 0x0b61},
		{0x0b85, 0x0b8a}, {0x0b8e, 0x0b90}, {0x0b92, 0x0b95}, {0x0b99, 0x0b9a}, {0x0b9c, 0x0b9c}, {0x0b9e, 0x0b9f}, {0x0ba3, 0x0ba4}, {0x0ba8, 0x0baa},
		{0x0bae, 0x0bb5}, {0x0bb7, 0x0bb9}, {0x0c05, 0x0c0c}, {0x0c0e, 0x0c10}, {0x0c12, 0x0c28}, {0x0c2a, 0x0c33}, {0x0c35, 0x0c39}, {0x0c60, 0x0c61},
		{0x0c85, 0x0c8c}, {0x0c8e, 0x0c90}, {0x0c92, 0x0ca8}, {0x0caa, 0x0cb3}, {0x0cb5, 0x0cb9}, {0x0ce0, 0x0ce1}, {0x0d05, 0x0d0c}, {0x0d0e, 0x0d10},
		{0x0d12, 0x0d28}, {0x0d2a, 0x0d39}, {0x0d60, 0x0d61}, {0x0e01, 0x0e30}, {0x0e32, 0x0e33}, {0x0e40, 0x0e46}, {0x0e4f, 0x0e5b}, {0x0e81, 0x0e82},
		{0x0e84, 0x0e84}, {0x0e87, 0x0e87}, {0x0e88, 0x0e88}, {0x0e8a, 0x0e8a}, {0x0e8d, 0x0e8d}, {0x0e94, 0x0e97}, {0x0e99, 0x0e9f}, {0x0ea1, 0x0ea3},
		{0x0ea5, 0x0ea5}, {0x0ea7, 0x0ea7}, {0x0eaa, 0x0eaa}, {0x0eab, 0x0eab}, {0x0ead, 0x0eb0}, {0x0eb2, 0x0eb2}, {0x0eb3, 0x0eb3}, {0x0ebd, 0x0ebd},
		{0x0ec0, 0x0ec4}, {0x0ec6, 0x0ec6}, {0x10a0, 0x10c5}, {0x10d0, 0x10f6}, {0x1100, 0x1159}, {0x1161, 0x11a2}, {0x11a8, 0x11f9}, {0x1e00, 0x1e9a},
		{0x1ea0, 0x1ef9}, {0x1f00, 0x1f15}, {0x1f18, 0x1f1d}, {0x1f20, 0x1f45}, {0x1f48, 0x1f4d}, {0x1f50, 0x1f57}, {0x1f59, 0x1f59}, {0x1f5b, 0x1f5b},
		{0x1f5d, 0x1f5d}, {0x1f5f, 0x1f7d}, {0x1f80, 0x1fb4}, {0x1fb6, 0x1fbc}, {0x1fc2, 0x1fc4}, {0x1fc6, 0x1fcc}, {0x1fd0, 0x1fd3}, {0x1fd6, 0x1fdb},
		{0x1fe0, 0x1fec}, {0x1ff2, 0x1ff4}, {0x1ff6, 0x1ffc}, {0x3041, 0x3094}, {0x309b, 0x309e}, {0x30a1, 0x30fe}, {0x3105, 0x312c}, {0x4e00, 0x9fa5},
		{0xf900, 0xfa2d}, {0xfb1f, 0xfb36}, {0xfb38, 0xfb3c}, {0xfb3e, 0xfb3e}, {0xfb40, 0xfb41}, {0xfb42, 0xfb44}, {0xfb46, 0xfbb1}, {0xfbd3, 0xfd3f},
		{0xfd50, 0xfd8f}, {0xfd92, 0xfdc7}, {0xfdf0, 0xfdfb}, {0xfe70, 0xfe72}, {0xfe74, 0xfe74}, {0xfe76, 0xfefc}, {0xff21, 0xff3a}, {0xff41, 0xff5a},
		{0xff66, 0xffbe}, {0xffc2, 0xffc7}, {0xffca, 0xffcf}, {0xffd2, 0xffd7}, {0xffda, 0xffdc}
	};
	const Uint N_UTF32_IDENTIFIER_RANGES = sizeof(UTF32_IDENTIFIER_RANGES) / sizeof(UTF32_IDENTIFIER_RANGES[0]);

	class Utf32IdentifierRangesSelfTest {
	public:
		Utf32IdentifierRangesSelfTest() {
			Uint32 last = 0;
			for (Uint i = 0; i < N_UTF32_IDENTIFIER_RANGES; i++) {
				ASSERT(UTF32_IDENTIFIER_RANGES[i].first >= last + 2);
				ASSERT(UTF32_IDENTIFIER_RANGES[i].last >= UTF32_IDENTIFIER_RANGES[i].first);
				last = UTF32_IDENTIFIER_RANGES[i].last;
			}
		}
	};
	const Utf32IdentifierRangesSelfTest utf32IdentifierRangesSelfTest();

	const Uint32 CHAR_EOF = (Uint32)-1;
}

Char::Char() :
	ch_(CHAR_EOF)
{
}

Char::Char(char ch) :
	ch_((Uint32)(Uint8)ch)
{
	ASSERT((Uint8)ch < ASCII_MAX);
}

bool Char::isUpper() const {
	return ((ch_ < ASCII_MAX) && ((PROPERTIES[ch_] & PROPERTY_UPPERCASE) != 0));
}

bool Char::isLower() const {
	return ((ch_ < ASCII_MAX) && ((PROPERTIES[ch_] & PROPERTY_LOWERCASE) != 0));
}

bool Char::isLetter() const {
	return ((ch_ < ASCII_MAX) && ((PROPERTIES[ch_] & (PROPERTY_UPPERCASE | PROPERTY_LOWERCASE)) != 0));
}

bool Char::isDecimalDigit() const {
	return ((ch_ < ASCII_MAX) && ((PROPERTIES[ch_] & PROPERTY_DIGIT) != 0));
}

bool Char::isBinaryDigit() const {
	return (ch_ >= (Uint8)'0') && (ch_ <= (Uint8)'1');
}

bool Char::isOctalDigit() const {
	return (ch_ >= (Uint8)'0') && (ch_ <= (Uint8)'7');
}

bool Char::isHexDigit() const {
	return ((ch_ < ASCII_MAX) && ((PROPERTIES[ch_] & (PROPERTY_DIGIT | PROPERTY_HEX_LETTER)) != 0));
}

bool Char::isWhitespace() const {
	return ((ch_ < ASCII_MAX) && ((PROPERTIES[ch_] & PROPERTY_WHITESPACE) != 0));
}

bool Char::isBasicSource() const {
	return ((ch_ < ASCII_MAX) && ((PROPERTIES[ch_] & (PROPERTY_UPPERCASE | PROPERTY_LOWERCASE | PROPERTY_DIGIT | PROPERTY_UNDERSCORE | PROPERTY_WHITESPACE | PROPERTY_OTHER_BASIC_SOURCE)) != 0));
}

bool Char::isPpIdentifier() const {
	if (ch_ < ASCII_MAX) {
		return ((PROPERTIES[ch_] & (PROPERTY_UPPERCASE | PROPERTY_LOWERCASE | PROPERTY_DIGIT | PROPERTY_UNDERSCORE)) != 0);
	}
	// ch_ could lie in one of the ranges from [lo] to [hi]
	Uint lo = 0;
	Uint hi = N_UTF32_IDENTIFIER_RANGES - 1;
	if (ch_ < UTF32_IDENTIFIER_RANGES[0].first) return false;
	if (ch_ > UTF32_IDENTIFIER_RANGES[N_UTF32_IDENTIFIER_RANGES - 1].last) return false;
	while (lo <= hi) {
		Uint mid = (lo + hi) / 2;
		if (ch_ < UTF32_IDENTIFIER_RANGES[mid].first) {
			// Cannot be -1 because of check at start
			hi = mid - 1;
		}
		else if (ch_ > UTF32_IDENTIFIER_RANGES[mid].last) {
			lo = mid + 1;
		}
		else {
			return true;
		}
	}
	return false;
}

bool Char::isEof() const {
	return ch_ == CHAR_EOF;
}

bool Char::isAscii() const {
	return ch_ < ASCII_MAX;
}

Uint Char::decimalValue() const {
	ASSERT(isDecimalDigit());
	return (Uint)(ch_ - (Uint8)'0');
}

Uint Char::binaryValue() const {
	ASSERT(isBinaryDigit());
	return (Uint)(ch_ - (Uint8)'0');
}

Uint Char::octalValue() const {
	ASSERT(isOctalDigit());
	return (Uint)(ch_ - (Uint8)'0');
}

Uint Char::hexValue() const {
	ASSERT(isHexDigit());
	if (isDecimalDigit()) {
		return (Uint)(ch_ - (Uint8)'0');
	}
	if (isUpper()) {
		return (Uint)(ch_ - (Uint8)'A') + 10u;
	}
	ASSERT(isLower());
	return (Uint)(ch_ - (Uint8)'a') + 10u;
}

Char Char::toUpperCopy() const {
	return Char(isLower() ? (ch_ - (Uint8)0x20) : ch_);
}

Char Char::toLowerCopy() const {
	return Char(isUpper() ? (ch_ + (Uint8)0x20) : ch_);
}

Char Char::eof() {
	return Char(CHAR_EOF);
}

Char Char::decimalDigit(Uint value) {
	ASSERT(value < 10);
	return Char((Uint32)'0' + value);
}

Char Char::octalDigit(Uint value) {
	ASSERT(value < 8);
	return Char((Uint32)'0' + value);
}

Char Char::hexDigit(Uint value, bool upperNotLower) {
	ASSERT(value < 16);
	if (value < 10) {
		return Char((Uint32)'0' + value);
	}
	else if (upperNotLower) {
		return Char((Uint32)'A' + value - 10u);
	}
	else {
		return Char((Uint32)'a' + value - 10u);
	}
}

Char Char::fromUtf8(const char* utf8, Uint& len) {
	if ((Uint8)utf8[0] < 0x80u) {
		// Handle 1 byte immediately for efficiency
		len = 1;
		return Char(utf8[0]);
	}
			
	Uint32 ch = (Uint32)(Uint8)utf8[0];

	// Determine the length from the first byte. A continuation byte on its
	// own is treated as an error character with a single byte.
	len = (ch >= 0xf0u) ? 4 :
		  (ch >= 0xe0u) ? 3 :
		  (ch >= 0xc0u) ? 2 : 1;

	// Shift the continuation bytes. The value includes the preamble bits from
	// the first byte. We stop with an error is any of the extra bytes are not
	// continuation bytes.
	for (Uint pos = 1; pos < len; pos++) {
		Uint32 x = (Uint32)(Uint8)utf8[pos];
		if ((x < 0x80u) || (x >= 0xc0u)) {
			// Not a continuation byte
			len = pos;
			return Char::eof();
		}
		ch = (ch << 6) | (x & 0x3fu);
	}
	
	switch (len) {
	case 1:
		return Char::eof();
	case 2:
		// Two byte character U+0080 to U+07FF
		// Preamble should be 0x3000
		if ((ch < 0x3080u) || (ch >= 0x3800u)) {
			return Char::eof();
		}
		ch &= ~0x3000u;
		break;
	case 3:
		// Three byte character U+0800 to U+FFFF excluding U+D800 to U+DFFF
		// Preamble should be 0x000e0000
		if ((ch < 0x000e0800u) || 
			((ch >= 0x000ed800u) && (ch < 0x000ee000u)) ||
			(ch >= 0x000f0000u))
		{
			return Char::eof();
		}
		ch &= ~0x000e0000;
		break;
	case 4:
		// Four byte character U+010000 to U+10FFFF
		// Preamble should be 0x03c00000
		if ((ch < 0x03c10000) || (ch >= 0x03d10000)) {
			return Char::eof();
		}
		ch &= ~0x03c00000;
		break;
	default:
		FAIL;
	}
		
	return Char(ch);
}

Char Char::fromUtf16(const char16_t* utf16, Uint& len) {
	if ((utf16[0] < 0xd800u) || (utf16[0] >= 0xe000u)) {
		len = 1;
		return Char((Uint32)utf16[0]);
	}
	if ((utf16[0] >= 0xdc00u) ||
		(utf16[1] < 0xdc00u) ||
		(utf16[1] >= 0xe000u))
	{
		len = 1;
		return Char(CHAR_EOF);
	}
	// So [0] is d800-dbff and [1] is dc00-dfff
	len = 2;
	return Char((Uint32)(((utf16[0] & 0x03ffu) << 10) | (utf16[1] & 0x03ffu)) + 0x10000u);
}

Char Char::fromUtf32(char32_t utf32) {
	if (((utf32 >= 0x0000d800u) && (utf32 < 0x0000e000u)) ||
		(utf32 >= 0x00110000))
	{
		return Char(CHAR_EOF);
	}
	return Char((Uint32)utf32);
}

Uint Char::toUtf8(char* utf8) {
	if (ch_ < 0x80u) {
		// One byte character U+0000 to U+007F
		utf8[0]= (char)ch_;
		return 1;
	}
	else if (ch_ < 0x0800u) {
		// Two byte character U+0080 to U+07FF
		utf8[0] = (char)(0xc0u + (ch_ >> 6));
		utf8[1] = (char)(0x80u + (ch_ & 0x3fu));
		return 2;
	}
	else if (ch_ < 0x00010000u) {
		// Three byte character U+0800 to U+FFFF excluding U+D800 to U+DFFF
		utf8[0] = (char)(0xe0u + (ch_ >> 12));
		utf8[1] = (char)(0x80u + ((ch_ >> 6) & 0x3fu));
		utf8[2] = (char)(0x80u + (ch_ & 0x3fu));
		return 3;
	}
	else {
		// Four byte character U+010000 to U+10FFFF
		ASSERT(ch_ != CHAR_EOF);
		utf8[0] = (char)(0xf0u + (ch_ >> 18));
		utf8[1] = (char)(0x80u + ((ch_ >> 12) & 0x3fu));
		utf8[2] = (char)(0x80u + ((ch_ >> 6) & 0x3fu));
		utf8[3] = (char)(0x80u + (ch_ & 0x3fu));
		return 4;
	}
}

Uint Char::toUtf16(char16_t* utf16) {
	if (ch_ < 0x010000u) {
		// One word character U+0000 to U+FFFF excluding U+D800 to U+DFFF
		utf16[0] = (char16_t)ch_;
		return 1;
	}
	else {
		// Two word character U+010000 to U+10FFFF
		ASSERT(ch_ != CHAR_EOF);
		utf16[0] = (char16_t)(0xd800u + ((ch_ - 0x10000u) >> 10));
		utf16[1] = (char16_t)(0xdc00u + (ch_ & 0x3ffu));
		return 2;
	}
}

char32_t Char::toUtf32() {
	return (char32_t)ch_;
}

bool Char::operator==(Char other) const {
	return ch_ == other.ch_;
}

bool Char::operator!=(Char other) const {
	return ch_ != other.ch_;
}

bool Char::operator==(char other) const {
	ASSERT((Uint)other < ASCII_MAX);
	return ch_ == (Uint32)other;
}

bool Char::operator!=(char other) const {
	ASSERT((Uint)other < ASCII_MAX);
	return ch_ != (Uint32)other;
}

Char::Char(Uint32 ch) :
	ch_(ch)
{
	ASSERT ((ch < 0x0000d800u) ||
		    ((ch >= 0x0000e000u) && (ch < 0x00110000u)) ||
		    (ch == CHAR_EOF));
}

OutputStream& operator<<(OutputStream& os, Char ch) {
	os.streamChar(ch);
	return os;
}
