#pragma once
#include <codecvt>
#include <cwchar>
#include <locale>
#include "Util/Assert.hpp"
#include "Util/Char/Utf16CharOutputConverter.hpp"
#include "Util/CharOutputConverter.hpp"
#include "Util/Def.hpp"

// A character output converter to the ANSI or OEM character encodings.

///////////////////////////////////////////////////////////////////////////////
// Windows
///////////////////////////////////////////////////////////////////////////////

#if BUILD(WINDOWS)

class LocaleCharOutputConverter : public CharOutputConverter {
public:
	LocaleCharOutputConverter(bool isAnsiNotOem) :
		CharOutputConverter(isAnsiNotOem ? CharEncoding::ANSI : CharEncoding::OEM),
		// The default locale. Note std::locale("") is not the same as std::locale().
		// The former is the default operating system locale. The latter is the
		// "C" locale.
		defaultLocale_(isAnsiNotOem ? "": ".OCP"),
		facet_(std::use_facet<CodecvtType>(defaultLocale_)),
		mbstate_(std::mbstate_t())
	{
		ASSERT(sizeof(wchar_t) == 2);
	}

	Uint convertChar(Char ch, char* dst) {
		// Convert the character to UTF16 wbuf
		wchar_t wbuf[3];
		Uint wlen = ch.toUtf16((char16_t *)wbuf);
		ASSERT(wlen <= 2);

		// Write to encoded outbuf
		char outBuf[5];
		Uint outBufLen = 0;
		const wchar_t* pwbuf = 0;
		char* pout = 0;
		CodecvtType::result codecvtResult = facet_.out(
				mbstate_,
				wbuf, &wbuf[wlen], pwbuf,
				outBuf, &outBuf[4], pout);
		outBufLen = (Uint)(pout - outBuf);
		ASSERT(outBufLen <= 4);
		if (outBufLen > 0) {
			// Success
			ASSERT(codecvtResult == CodecvtType::ok);
			ASSERT(pwbuf == &wbuf[wlen]);
			memcpy(dst, outBuf, outBufLen);
		}
		else {
			// Failure
			ASSERT(codecvtResult == CodecvtType::error);
			ASSERT(pwbuf == wbuf);
		}
		return outBufLen;
	}

private:
	std::locale defaultLocale_;
	typedef std::codecvt<wchar_t, char, std::mbstate_t> CodecvtType;
	const CodecvtType& facet_;
	std::mbstate_t mbstate_;
};

#endif

///////////////////////////////////////////////////////////////////////////////
// Linux
///////////////////////////////////////////////////////////////////////////////

#if BUILD(LINUX)

#error TODO

#endif
