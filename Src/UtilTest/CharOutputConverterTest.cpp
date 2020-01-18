#include "Util/CharOutputConverter.hpp"
#include "TestTool/TestUtil.hpp"
#include "Util/SystemCout.hpp"

// Note these tests correspond loosely with those in CharInputConverterTest.

static void testChar(CharOutputConverterPtr cv, Char ch, char* expDst) {
	char dst[CharOutputConverter::MAX_OUTPUT_CHAR_BYTES + 1];
	Uint len = cv->convertChar(ch, dst);
	ASSERT(len <= CharOutputConverter::MAX_OUTPUT_CHAR_BYTES);
	dst[len] = '\0';
	int cmp = strcmp(dst, expDst);
	CHECK(cmp == 0);
	if (cmp != 0) {
		scout << "Expected string: " << expDst << sendl;
	}
}
	
// ANSI
AUTO_TEST_CASE {
	CharOutputConverterPtr cv = CharOutputConverter::create(CharEncoding::ANSI);
	CHECK(cv->getDstEncoding() == CharEncoding::ANSI);
	CHECK(cv->getDstEncoding() == CharEncoding::DEFAULT);

	// Normal conversion
	// 0x80 is euro symbol U+20ac
	// 0x86 is dagger U+2020
	// 0xa3 is pound U+00a3
	// 0xf7 is divide U+00f7
	testChar(cv, Char('H'), "H");
	testChar(cv, Char('e'), "e");
	testChar(cv, Char('\n'), "\n");
	testChar(cv, Char('\r'), "\r");
	testChar(cv, Char('\0'), "\0");
	testChar(cv, Char::fromUtf32(0x20ac), "\x80");
	testChar(cv, Char::fromUtf32(0x2020), "\x86");
	testChar(cv, Char::fromUtf32(0x00a3), "\xa3");
	testChar(cv, Char::fromUtf32(0x00f7), "\xf7");
	testChar(cv, Char('!'), "!");

	// Convenience functions
	std::string dst = "fred";
	cv->convertAppend("H\n\rW\xe2\x82\xac\xe2\x80\xa0\xc2\xa3\xc3\xb7!", dst);
	CHECK(dst == "fredH\n\rW\x80\x86\xa3\xf7!");
	CHECK(cv->convertString(String("H\n\rW\xe2\x82\xac\xe2\x80\xa0\xc2\xa3\xc3\xb7!"))
		== "H\n\rW\x80\x86\xa3\xf7!");
}

// OEM
AUTO_TEST_CASE {
	CharOutputConverterPtr cv = CharOutputConverter::create(CharEncoding::OEM);
	CHECK(cv->getDstEncoding() == CharEncoding::OEM);

	// Normal conversion
	// 0xf2 is ? U+2017
	// 0xb1 is ? U+2592
	// 0x9c is pound U+00a3
	// 0xf6 is divide U+00f7
	testChar(cv, Char('H'), "H");
	testChar(cv, Char('e'), "e");
	testChar(cv, Char('\n'), "\n");
	testChar(cv, Char('\r'), "\r");
	testChar(cv, Char('\0'), "\0");
	testChar(cv, Char::fromUtf32(0x2017), "\xf2");
	testChar(cv, Char::fromUtf32(0x2592), "\xb1");
	testChar(cv, Char::fromUtf32(0x00a3), "\x9c");
	testChar(cv, Char::fromUtf32(0x00f7), "\xf6");
	testChar(cv, Char('!'), "!");

	// Convenience functions
	std::string dst = "fred";
	cv->convertAppend("H\n\rW\xe2\x80\x97\xe2\x96\x92\xc2\xa3\xc3\xb7!", dst);
	CHECK(dst == "fredH\n\rW\xf2\xb1\x9c\xf6!");
	CHECK(cv->convertString(String("H\n\rW\xe2\x80\x97\xe2\x96\x92\xc2\xa3\xc3\xb7!"))
		== "H\n\rW\xf2\xb1\x9c\xf6!");
}

// UTF8
AUTO_TEST_CASE {
	CharOutputConverterPtr cv = CharOutputConverter::create(CharEncoding::UTF8);
	CHECK(cv->getDstEncoding() == CharEncoding::UTF8);

	// Normal conversion
	// \xc2\xa2         is U+00a2
	// \xe2\x82\xac     is U+20ac
	// \xf0\xa4\xad\xa2 is U+24b62
	testChar(cv, Char('H'), "H");
	testChar(cv, Char('e'), "e");
	testChar(cv, Char('\n'), "\n");
	testChar(cv, Char('\r'), "\r");
	testChar(cv, Char('\0'), "\0");
	testChar(cv, Char::fromUtf32(0x00a2), "\xc2\xa2");
	testChar(cv, Char::fromUtf32(0x20ac), "\xe2\x82\xac");
	testChar(cv, Char::fromUtf32(0x24b62), "\xf0\xa4\xad\xa2");
	testChar(cv, Char('!'), "!");
}

// UTF16BE
AUTO_TEST_CASE {
	CharOutputConverterPtr cv = CharOutputConverter::create(CharEncoding::UTF16BE);
	CHECK(cv->getDstEncoding() == CharEncoding::UTF16BE);

	// Normal conversion
	// 00 7a       is U+007a (z)
	// 6c 34       is U+6c34
	// d8 34 dd 1e is U+1d11e
	testChar(cv, Char('z'), "\x00\x7a");
	testChar(cv, Char::fromUtf32(0x6c34), "\x6c\x34");
	testChar(cv, Char::fromUtf32(0x1d11e), "\xd8\x34\xdd\x1e");
}

// UTF16LE
AUTO_TEST_CASE {
	CharOutputConverterPtr cv = CharOutputConverter::create(CharEncoding::UTF16LE);
	CHECK(cv->getDstEncoding() == CharEncoding::UTF16LE);

	// Normal conversion
	// 7a 00       is U+007a (z)
	// 34 6c       is U+6c34
	// 34 d8 1e dd is U+1d11e
	testChar(cv, Char('z'), "\x7a\x00");
	testChar(cv, Char::fromUtf32(0x6c34), "\x34\x6c");
	testChar(cv, Char::fromUtf32(0x1d11e), "\x34\xd8\x1e\xdd");
}

// UTF32BE
AUTO_TEST_CASE {
	CharOutputConverterPtr cv = CharOutputConverter::create(CharEncoding::UTF32BE);
	CHECK(cv->getDstEncoding() == CharEncoding::UTF32BE);

	// Normal conversion
	// 00 00 00 7a       is U+007a (z)
	// 00 00 6c 34       is U+6c34
	// 00 01 d1 1e       is U+1d11e
	testChar(cv, Char('z'), "\x00\x00\x00\x7a");
	testChar(cv, Char::fromUtf32(0x6c34), "\x00\x00\x6c\x34");
	testChar(cv, Char::fromUtf32(0x1d11e), "\x00\x01\xd1\x1e");
}

// UTF32LE
AUTO_TEST_CASE {
	CharOutputConverterPtr cv = CharOutputConverter::create(CharEncoding::UTF32LE);
	CHECK(cv->getDstEncoding() == CharEncoding::UTF32LE);

	// Normal conversion
	// 7a 00 00 00       is U+007a (z)
	// 34 6c 00 00       is U+6c34
	// 1e d1 01 00       is U+1d11e
	testChar(cv, Char('z'), "\x7a\x00\x00\x00");
	testChar(cv, Char::fromUtf32(0x6c34), "\x34\x6c\x00\x00");
	testChar(cv, Char::fromUtf32(0x1d11e), "\x1e\xd1\x01\x00");
}
