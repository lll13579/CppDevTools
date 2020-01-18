#include "Util/Char.hpp"
#include "TestTool/TestUtil.hpp"

// Test character catagorisation functions.
AUTO_TEST_CASE {
	Char ch = 'A';
	CHECK( ch.isUpper());
	CHECK(!ch.isLower());
	CHECK( ch.isLetter());
	CHECK(!ch.isDecimalDigit());
	CHECK(!ch.isOctalDigit());
	CHECK( ch.isHexDigit());
	CHECK(!ch.isWhitespace());
	CHECK( ch.isBasicSource());
	CHECK( ch.isPpIdentifier());
	CHECK(!ch.isEof());
	CHECK( ch.isAscii());
	CHECK(ch.hexValue() == 10);
	CHECK(ch.toUpperCopy() == 'A');
	CHECK(ch.toLowerCopy() == 'a');

	ch = 'Z';
	CHECK( ch.isUpper());
	CHECK(!ch.isLower());
	CHECK( ch.isLetter());
	CHECK(!ch.isDecimalDigit());
	CHECK(!ch.isOctalDigit());
	CHECK(!ch.isHexDigit());
	CHECK(!ch.isWhitespace());
	CHECK( ch.isBasicSource());
	CHECK( ch.isPpIdentifier());
	CHECK(!ch.isEof());
	CHECK( ch.isAscii());
	CHECK(ch.toUpperCopy() == 'Z');
	CHECK(ch.toLowerCopy() == 'z');

	ch = 'a';
	CHECK(!ch.isUpper());
	CHECK( ch.isLower());
	CHECK( ch.isLetter());
	CHECK(!ch.isDecimalDigit());
	CHECK(!ch.isOctalDigit());
	CHECK( ch.isHexDigit());
	CHECK(!ch.isWhitespace());
	CHECK( ch.isBasicSource());
	CHECK( ch.isPpIdentifier());
	CHECK(!ch.isEof());
	CHECK( ch.isAscii());
	CHECK(ch.hexValue() == 10);
	CHECK(ch.toUpperCopy() == 'A');
	CHECK(ch.toLowerCopy() == 'a');

	ch = 'z';
	CHECK(!ch.isUpper());
	CHECK( ch.isLower());
	CHECK( ch.isLetter());
	CHECK(!ch.isDecimalDigit());
	CHECK(!ch.isOctalDigit());
	CHECK(!ch.isHexDigit());
	CHECK(!ch.isWhitespace());
	CHECK( ch.isBasicSource());
	CHECK( ch.isPpIdentifier());
	CHECK(!ch.isEof());
	CHECK( ch.isAscii());
	CHECK(ch.toUpperCopy() == 'Z');
	CHECK(ch.toLowerCopy() == 'z');

	ch = '0';
	CHECK(!ch.isUpper());
	CHECK(!ch.isLower());
	CHECK(!ch.isLetter());
	CHECK( ch.isDecimalDigit());
	CHECK( ch.isOctalDigit());
	CHECK( ch.isHexDigit());
	CHECK(!ch.isWhitespace());
	CHECK( ch.isBasicSource());
	CHECK( ch.isPpIdentifier());
	CHECK(!ch.isEof());
	CHECK( ch.isAscii());
	CHECK(ch.decimalValue() == 0);
	CHECK(ch.octalValue() == 0);
	CHECK(ch.hexValue() == 0);
	CHECK(ch.toUpperCopy() == ch);
	CHECK(ch.toLowerCopy() == ch);

	ch = '9';
	CHECK(!ch.isUpper());
	CHECK(!ch.isLower());
	CHECK(!ch.isLetter());
	CHECK( ch.isDecimalDigit());
	CHECK(!ch.isOctalDigit());
	CHECK( ch.isHexDigit());
	CHECK(!ch.isWhitespace());
	CHECK( ch.isBasicSource());
	CHECK( ch.isPpIdentifier());
	CHECK(!ch.isEof());
	CHECK( ch.isAscii());
	CHECK(ch.decimalValue() == 9);
	CHECK(ch.toUpperCopy() == ch);
	CHECK(ch.toLowerCopy() == ch);

	ch = '_';
	CHECK(!ch.isUpper());
	CHECK(!ch.isLower());
	CHECK(!ch.isLetter());
	CHECK(!ch.isDecimalDigit());
	CHECK(!ch.isOctalDigit());
	CHECK(!ch.isHexDigit());
	CHECK(!ch.isWhitespace());
	CHECK( ch.isBasicSource());
	CHECK( ch.isPpIdentifier());
	CHECK(!ch.isEof());
	CHECK( ch.isAscii());
	CHECK(ch.toUpperCopy() == ch);
	CHECK(ch.toLowerCopy() == ch);

	ch = '%';
	CHECK(!ch.isUpper());
	CHECK(!ch.isLower());
	CHECK(!ch.isLetter());
	CHECK(!ch.isDecimalDigit());
	CHECK(!ch.isOctalDigit());
	CHECK(!ch.isHexDigit());
	CHECK(!ch.isWhitespace());
	CHECK( ch.isBasicSource());
	CHECK(!ch.isPpIdentifier());
	CHECK(!ch.isEof());
	CHECK( ch.isAscii());
	CHECK(ch.toUpperCopy() == ch);
	CHECK(ch.toLowerCopy() == ch);

	ch = '$';
	CHECK(!ch.isUpper());
	CHECK(!ch.isLower());
	CHECK(!ch.isLetter());
	CHECK(!ch.isDecimalDigit());
	CHECK(!ch.isOctalDigit());
	CHECK(!ch.isHexDigit());
	CHECK(!ch.isWhitespace());
	CHECK(!ch.isBasicSource());
	CHECK(!ch.isPpIdentifier());
	CHECK(!ch.isEof());
	CHECK( ch.isAscii());
	CHECK(ch.toUpperCopy() == ch);
	CHECK(ch.toLowerCopy() == ch);

	// From C++ identifier list - Greek character
	ch = Char::fromUtf32(0x1fd1u);
	CHECK(!ch.isUpper());
	CHECK(!ch.isLower());
	CHECK(!ch.isLetter());
	CHECK(!ch.isDecimalDigit());
	CHECK(!ch.isOctalDigit());
	CHECK(!ch.isHexDigit());
	CHECK(!ch.isWhitespace());
	CHECK(!ch.isBasicSource());
	CHECK( ch.isPpIdentifier());
	CHECK(!ch.isEof());
	CHECK(!ch.isAscii());
	CHECK(ch.toUpperCopy() == ch);
	CHECK(ch.toLowerCopy() == ch);

	// From UTF8 wikipedia entry - asian character
	ch = Char::fromUtf32(0x24b62u);
	CHECK(!ch.isUpper());
	CHECK(!ch.isLower());
	CHECK(!ch.isLetter());
	CHECK(!ch.isDecimalDigit());
	CHECK(!ch.isOctalDigit());
	CHECK(!ch.isHexDigit());
	CHECK(!ch.isWhitespace());
	CHECK(!ch.isBasicSource());
	CHECK(!ch.isPpIdentifier());
	CHECK(!ch.isEof());
	CHECK(!ch.isAscii());
	CHECK(ch.toUpperCopy() == ch);
	CHECK(ch.toLowerCopy() == ch);

	ch = Char::eof();
	CHECK(!ch.isUpper());
	CHECK(!ch.isLower());
	CHECK(!ch.isLetter());
	CHECK(!ch.isDecimalDigit());
	CHECK(!ch.isOctalDigit());
	CHECK(!ch.isHexDigit());
	CHECK(!ch.isWhitespace());
	CHECK(!ch.isBasicSource());
	CHECK(!ch.isPpIdentifier());
	CHECK( ch.isEof());
	CHECK(!ch.isAscii());
	CHECK(ch.toUpperCopy() == ch);
	CHECK(ch.toLowerCopy() == ch);

	CHECK( Char(' ').isWhitespace());
	CHECK( Char('\t').isWhitespace());
	CHECK( Char('\v').isWhitespace());
	CHECK( Char('\f').isWhitespace());
	CHECK(!Char('\r').isWhitespace());
	CHECK( Char('\n').isWhitespace());

	// There are exactly 96 characters in the basic source character set
	Uint count = 0;
	for (Uint i = 0; i < 128; i++) {
		if (Char((char)i).isBasicSource()) count++;
	}
	CHECK(count == 96);
}

// Test digit creation functions
AUTO_TEST_CASE {
	CHECK(Char::decimalDigit(0) == '0');
	CHECK(Char::decimalDigit(9) == '9');

	CHECK(Char::octalDigit(0) == '0');
	CHECK(Char::octalDigit(7) == '7');

	CHECK(Char::hexDigit(0, true) == '0');
	CHECK(Char::hexDigit(9, true) == '9');
	CHECK(Char::hexDigit(10, true) == 'A');
	CHECK(Char::hexDigit(15, true) == 'F');
	CHECK(Char::hexDigit(0, false) == '0');
	CHECK(Char::hexDigit(9, false) == '9');
	CHECK(Char::hexDigit(10, false) == 'a');
	CHECK(Char::hexDigit(15, false) == 'f');
}

// Test all Unicode characters
AUTO_TEST_CASE {
	for (char32_t value = 0u; value < 0x10ffffu + 16u; value++) {
		Char ch = Char::fromUtf32(value);
		if ((value >= 0xd800u) && (value <= 0xdfffu)) {
			CHECK(ch.isEof());
			continue;
		}
		if (value > 0x10ffffu) {
			CHECK(ch.isEof());
			continue;
		}
		CHECK(!ch.isEof());

		CHECK(ch.toUtf32() == value);

		Uint expectedUtf8Len = (value < 0x80u) ? 1 :
							   (value < 0x800u) ? 2:
							   (value < 0x10000u) ? 3 : 4;
		Uint expectedUtf16Len = (value < 0x10000u) ? 1 : 2;

		char utf8[5];
		utf8[expectedUtf8Len] = '\x3c';
		Uint utf8Len = ch.toUtf8(utf8);
		CHECK(utf8Len == expectedUtf8Len);
		CHECK(utf8[expectedUtf8Len] == '\x3c');
		utf8[utf8Len] = '\x80'; // Use a continuation byte to try to confuse

		char16_t utf16[3];
		utf16[expectedUtf16Len] = 0x3c4d;
		Uint utf16Len = ch.toUtf16(utf16);
		CHECK(utf16Len == expectedUtf16Len);
		CHECK(utf16[expectedUtf16Len] == 0x3c4d);
		utf16[utf16Len] = 0xdc00; // Use a continuation word to try to confuse

		Uint fromUtf8Len = 0;
		Char ch8 = Char::fromUtf8(utf8, fromUtf8Len);
		CHECK(fromUtf8Len == expectedUtf8Len);
		CHECK(ch8 == ch);

		Uint fromUtf16Len = 0;
		Char ch16 = Char::fromUtf16(utf16, fromUtf16Len);
		CHECK(fromUtf16Len == expectedUtf16Len);
		CHECK(ch16 == ch);
	}
}
