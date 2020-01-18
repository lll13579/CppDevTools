#include "Util/CharInputConverter.hpp"
#include "TestTool/TestUtil.hpp"

// ANSI
AUTO_TEST_CASE {
	CharInputConverterPtr cv = CharInputConverter::create(
		CharEncoding::ANSI, CharInputConverterErrorHandlerPtr(), Char::eof());
	CHECK(cv->getSrcEncoding() == CharEncoding::ANSI);
	CHECK(cv->getSrcEncoding() == CharEncoding::DEFAULT);

	// Normal conversion
	// 0x80 is euro symbol U+20ac
	// 0x86 is dagger U+2020
	// 0xa3 is pound U+00a3
	// 0xf7 is divide U+00f7
	CHECK(cv->convertChar("He\n\r\0Wo\x80\x86\xa3\xf7!", 12u) == std::make_pair(1u, Char('H')));
	CHECK(cv->convertChar("e\n\r\0Wo\x80\x86\xa3\xf7!", 11u) == std::make_pair(1u, Char('e')));
	CHECK(cv->convertChar("\n\r\0Wo\x80\x86\xa3\xf7!", 10u) == std::make_pair(1u, Char('\n')));
	CHECK(cv->convertChar("\r\0Wo\x80\x86\xa3\xf7!", 9u) == std::make_pair(1u, Char('\r')));
	CHECK(cv->convertChar("\0Wo\x80\x86\xa3\xf7!", 8u) == std::make_pair(1u, Char('\0')));
	CHECK(cv->convertChar("Wo\x80\x86\xa3\xf7!", 7u) == std::make_pair(1u, Char('W')));
	CHECK(cv->convertChar("o\x80\x86\xa3\xf7!", 6u) == std::make_pair(1u, Char('o')));
	CHECK(cv->convertChar("\x80\x86\xa3\xf7!", 5u) == std::make_pair(1u, Char::fromUtf32(0x20ac)));
	CHECK(cv->convertChar("\x86\xa3\xf7!", 4u) == std::make_pair(1u, Char::fromUtf32(0x2020)));
	CHECK(cv->convertChar("\xa3\xf7!", 3u) == std::make_pair(1u, Char::fromUtf32(0x00a3)));
	CHECK(cv->convertChar("\xf7!", 2u) == std::make_pair(1u, Char::fromUtf32(0x00f7)));
	CHECK(cv->convertChar("!", 1u) == std::make_pair(1u, Char('!')));

	// Convenience functions
	String dst = "fred";
	cv->convertAppend("H\n\rW\x80\x86\xa3\xf7!", 9u, dst);
	CHECK(dst == "fredH\n\rW\xe2\x82\xac\xe2\x80\xa0\xc2\xa3\xc3\xb7!");
	dst = "fred";
	cv->convertAppend(std::string("H\n\rW\x80\x86\xa3\xf7!"), dst);
	CHECK(dst == "fredH\n\rW\xe2\x82\xac\xe2\x80\xa0\xc2\xa3\xc3\xb7!");
	CHECK(cv->convertString("H\n\rW\x80\x86\xa3\xf7!", 9u) == String("H\n\rW\xe2\x82\xac\xe2\x80\xa0\xc2\xa3\xc3\xb7!"));
	CHECK(cv->convertString(std::string("H\n\rW\x80\x86\xa3\xf7!")) == String("H\n\rW\xe2\x82\xac\xe2\x80\xa0\xc2\xa3\xc3\xb7!"));
}

// OEM
AUTO_TEST_CASE {
	CharInputConverterPtr cv = CharInputConverter::create(
		CharEncoding::OEM, CharInputConverterErrorHandlerPtr(), Char::eof());
	CHECK(cv->getSrcEncoding() == CharEncoding::OEM);

	// Normal conversion
	// 0xf2 is ? U+2017
	// 0xb1 is ? U+2592
	// 0x9c is pound U+00a3
	// 0xf6 is divide U+00f7
	CHECK(cv->convertChar("He\n\r\0Wo\xf2\xb1\x9c\xf6!", 12u) == std::make_pair(1u, Char('H')));
	CHECK(cv->convertChar("e\n\r\0Wo\xf2\xb1\x9c\xf6!", 11u) == std::make_pair(1u, Char('e')));
	CHECK(cv->convertChar("\n\r\0Wo\xf2\xb1\x9c\xf6!", 10u) == std::make_pair(1u, Char('\n')));
	CHECK(cv->convertChar("\r\0Wo\xf2\xb1\x9c\xf6!", 9u) == std::make_pair(1u, Char('\r')));
	CHECK(cv->convertChar("\0Wo\xf2\xb1\x9c\xf6!", 8u) == std::make_pair(1u, Char('\0')));
	CHECK(cv->convertChar("Wo\xf2\xb1\x9c\xf6!", 7u) == std::make_pair(1u, Char('W')));
	CHECK(cv->convertChar("o\xf2\xb1\x9c\xf6!", 6u) == std::make_pair(1u, Char('o')));
	CHECK(cv->convertChar("\xf2\xb1\x9c\xf6!", 5u) == std::make_pair(1u, Char::fromUtf32(0x2017)));
	CHECK(cv->convertChar("\xb1\x9c\xf6!", 4u) == std::make_pair(1u, Char::fromUtf32(0x2592)));
	CHECK(cv->convertChar("\x9c\xf6!", 3u) == std::make_pair(1u, Char::fromUtf32(0x00a3)));
	CHECK(cv->convertChar("\xf6!", 2u) == std::make_pair(1u, Char::fromUtf32(0x00f7)));
	CHECK(cv->convertChar("!", 1u) == std::make_pair(1u, Char('!')));

	// Convenience functions
	String dst = "fred";
	cv->convertAppend("H\n\rW\xf2\xb1\x9c\xf6!", 9u, dst);
	CHECK(dst == "fredH\n\rW\xe2\x80\x97\xe2\x96\x92\xc2\xa3\xc3\xb7!");
	dst = "fred";
	cv->convertAppend(std::string("H\n\rW\xf2\xb1\x9c\xf6!"), dst);
	CHECK(dst == "fredH\n\rW\xe2\x80\x97\xe2\x96\x92\xc2\xa3\xc3\xb7!");
	CHECK(cv->convertString("H\n\rW\xf2\xb1\x9c\xf6!", 9u) == String("H\n\rW\xe2\x80\x97\xe2\x96\x92\xc2\xa3\xc3\xb7!"));
	CHECK(cv->convertString(std::string("H\n\rW\xf2\xb1\x9c\xf6!")) == String("H\n\rW\xe2\x80\x97\xe2\x96\x92\xc2\xa3\xc3\xb7!"));
}

// UTF8
AUTO_TEST_CASE {
	CharInputConverterPtr cv = CharInputConverter::create(CharEncoding::UTF8, CharInputConverterErrorHandlerPtr(), Char::eof());
	CHECK(cv->getSrcEncoding() == CharEncoding::UTF8);

	// Normal conversion
	// \xc2\xa2         is U+00a2
	// \xe2\x82\xac     is U+20ac
	// \xf0\xa4\xad\xa2 is U+24b62
	CHECK(cv->convertChar("He\n\r\0Wo\xc2\xa2\xe2\x82\xac\xf0\xa4\xad\xa2!", 17u) == std::make_pair(1u, Char('H')));
	CHECK(cv->convertChar("e\n\r\0Wo\xc2\xa2\xe2\x82\xac\xf0\xa4\xad\xa2!", 16u) == std::make_pair(1u, Char('e')));
	CHECK(cv->convertChar("\n\r\0Wo\xc2\xa2\xe2\x82\xac\xf0\xa4\xad\xa2!", 15u) == std::make_pair(1u, Char('\n')));
	CHECK(cv->convertChar("\r\0Wo\xc2\xa2\xe2\x82\xac\xf0\xa4\xad\xa2!", 14u) == std::make_pair(1u, Char('\r')));
	CHECK(cv->convertChar("\0Wo\xc2\xa2\xe2\x82\xac\xf0\xa4\xad\xa2!", 13u) == std::make_pair(1u, Char('\0')));
	CHECK(cv->convertChar("Wo\xc2\xa2\xe2\x82\xac\xf0\xa4\xad\xa2!", 12u) == std::make_pair(1u, Char('W')));
	CHECK(cv->convertChar("o\xc2\xa2\xe2\x82\xac\xf0\xa4\xad\xa2!", 11u) == std::make_pair(1u, Char('o')));
	CHECK(cv->convertChar("\xc2\xa2\xe2\x82\xac\xf0\xa4\xad\xa2!", 10u) == std::make_pair(2u, Char::fromUtf32(0x00a2)));
	CHECK(cv->convertChar("\xe2\x82\xac\xf0\xa4\xad\xa2!", 8u) == std::make_pair(3u, Char::fromUtf32(0x20ac)));
	CHECK(cv->convertChar("\xf0\xa4\xad\xa2!", 5u) == std::make_pair(4u, Char::fromUtf32(0x24b62)));
	CHECK(cv->convertChar("!", 1u) == std::make_pair(1u, Char('!')));

	// Illegal character
	CHECK(cv->convertChar("He\xa4\xadllo", 7u) == std::make_pair(1u, Char('H')));
	CHECK(cv->convertChar("\xa4\xadllo", 5u) == std::make_pair(1u, Char::eof()));
	CHECK(cv->convertChar("\xadllo", 4u) == std::make_pair(1u, Char::eof()));
	CHECK(cv->convertChar("llo", 3u) == std::make_pair(1u, Char('l')));

	// Partial character
	CHECK(cv->convertChar("H\xf0\xa4\xad", 4u) == std::make_pair(1u, Char('H')));
	CHECK(cv->convertChar("\xf0\xa4\xad", 3u) == std::make_pair(3u, Char::eof()));
}

// UTF16BE
AUTO_TEST_CASE {
	CharInputConverterPtr cv = CharInputConverter::create(CharEncoding::UTF16BE, CharInputConverterErrorHandlerPtr(), Char::eof());
	CHECK(cv->getSrcEncoding() == CharEncoding::UTF16BE);

	// Normal conversion
	// 00 7a       is U+007a (z)
	// 6c 34       is U+6c34
	// d8 34 dd 1e is U+1d11e
	CHECK(cv->convertChar("\x00\x7a\x6c\x34\xd8\x34\xdd\x1e\x00\x7a", 10u) == std::make_pair(2u, Char('z')));
	CHECK(cv->convertChar("\x6c\x34\xd8\x34\xdd\x1e\x00\x7a", 8u) == std::make_pair(2u, Char::fromUtf32(0x6c34)));
	CHECK(cv->convertChar("\xd8\x34\xdd\x1e\x00\x7a", 6u) == std::make_pair(4u, Char::fromUtf32(0x1d11e)));
	CHECK(cv->convertChar("\x00\x7a", 2u) == std::make_pair(2u, Char('z')));
}

// UTF16LE
AUTO_TEST_CASE {
	CharInputConverterPtr cv = CharInputConverter::create(CharEncoding::UTF16LE, CharInputConverterErrorHandlerPtr(), Char::eof());
	CHECK(cv->getSrcEncoding() == CharEncoding::UTF16LE);

	// Normal conversion
	std::pair<Uint, bool> result;
	// 7a 00       is U+007a (z)
	// 34 6c       is U+6c34
	// 34 d8 1e dd is U+1d11e
	CHECK(cv->convertChar("\x7a\x00\x34\x6c\x34\xd8\x1e\xdd\x7a\x00", 10u) == std::make_pair(2u, Char('z')));
	CHECK(cv->convertChar("\x34\x6c\x34\xd8\x1e\xdd\x7a\x00", 8u) == std::make_pair(2u, Char::fromUtf32(0x6c34)));
	CHECK(cv->convertChar("\x34\xd8\x1e\xdd\x7a\x00", 6u) == std::make_pair(4u, Char::fromUtf32(0x1d11e)));
	CHECK(cv->convertChar("\x7a\x00", 2u) == std::make_pair(2u, Char('z')));
}

// UTF32BE
AUTO_TEST_CASE {
	CharInputConverterPtr cv = CharInputConverter::create(CharEncoding::UTF32BE, CharInputConverterErrorHandlerPtr(), Char::eof());
	CHECK(cv->getSrcEncoding() == CharEncoding::UTF32BE);

	// Normal conversion
	// 00 00 00 7a       is U+007a (z)
	// 00 00 6c 34       is U+6c34
	// 00 01 d1 1e       is U+1d11e
	CHECK(cv->convertChar("\x00\x00\x00\x7a\x00\x00\x6c\x34\x00\x01\xd1\x1e\x00\x00\x00\x7a", 16u) == std::make_pair(4u, Char('z')));
	CHECK(cv->convertChar("\x00\x00\x6c\x34\x00\x01\xd1\x1e\x00\x00\x00\x7a", 12u) == std::make_pair(4u, Char::fromUtf32(0x6c34)));
	CHECK(cv->convertChar("\x00\x01\xd1\x1e\x00\x00\x00\x7a", 8u) == std::make_pair(4u, Char::fromUtf32(0x1d11e)));
	CHECK(cv->convertChar("\x00\x00\x00\x7a", 4u) == std::make_pair(4u, Char('z')));
}

// UTF32LE
AUTO_TEST_CASE {
	CharInputConverterPtr cv = CharInputConverter::create(CharEncoding::UTF32LE, CharInputConverterErrorHandlerPtr(), Char::eof());
	CHECK(cv->getSrcEncoding() == CharEncoding::UTF32LE);

	// Normal conversion
	// 7a 00 00 00       is U+007a (z)
	// 34 6c 00 00       is U+6c34
	// 1e d1 01 00       is U+1d11e
	CHECK(cv->convertChar("\x7a\x00\x00\x00\x34\x6c\x00\x00\x1e\xd1\x01\x00\x7a\x00\x00\x00", 16u) == std::make_pair(4u, Char('z')));
	CHECK(cv->convertChar("\x34\x6c\x00\x00\x1e\xd1\x01\x00\x7a\x00\x00\x00", 12u) == std::make_pair(4u, Char::fromUtf32(0x6c34)));
	CHECK(cv->convertChar("\x1e\xd1\x01\x00\x7a\x00\x00\x00", 8u) == std::make_pair(4u, Char::fromUtf32(0x1d11e)));
	CHECK(cv->convertChar("\x7a\x00\x00\x00", 4u) == std::make_pair(4u, Char('z')));
}
