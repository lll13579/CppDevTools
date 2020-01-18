#pragma once
#include <algorithm>
#include <codecvt>
#include <cwchar>
#include <locale>
#include "Util/Assert.hpp"
#include "Util/CharInputConverter.hpp"
#include "Util/Def.hpp"

#if BUILD(LINUX)
#error TODO
#endif

// A character input converter from a code page character encoding
// which in Windows can be either the ANSI or OEM code page for
// the local user.
//
// For efficiency, the conversion of single byte input characters is
// cached in a lookup table.

class LocaleCharInputConverter : public CharInputConverter {
public:
	LocaleCharInputConverter(
		bool isAnsiNotOem,
		CharInputConverterErrorHandlerPtr errorHandler, 
		Char errorChar) 
		:
		CharInputConverter(
			(isAnsiNotOem ? CharEncoding::ANSI : CharEncoding::OEM), 
			errorHandler, 
			errorChar),
		defaultLocale_(isAnsiNotOem ? "": ".OCP"),
		facet_(std::use_facet<CodecvtType>(defaultLocale_)),
		mbstate_(std::mbstate_t())
		// useMap_
		// map_
	{
		// Build up the map
		for (Uint ch = 0; ch < MAP_SIZE; ch++) {
			char in = (char)ch;
			std::pair<Char, bool> tcc = tryConvertChar(&in, 1u);
			useMap_[ch] = tcc.second;
			map_[ch] = tcc.first;
		}
	}

	std::pair<Uint, Char> convertChar(const char* src, Uint srcLen) {
		ASSERT(srcLen > 0u);

		Uint mapIndex = (Uint)*(const Uint8*)src;
		if (useMap_[mapIndex]) {
			// This is a single byte source character
			Char ch = map_[mapIndex];
			if (ch.isEof()) {
				reportError(src, 1u);
				return std::make_pair(1u, errorChar_);
			}
			return std::make_pair(1u, ch);
		}

		for (Uint testSrcLen = 2u; ; testSrcLen++) {
			std::pair<Char, bool> tcc = tryConvertChar(src, testSrcLen);
			if (tcc.second) {
				// This character is complete
				if (tcc.first.isEof()) {
					// But was an invalid character
					reportError(src, testSrcLen);
					return std::make_pair(testSrcLen, errorChar_);
				}
				else {
					return std::make_pair(testSrcLen, tcc.first);
				}
			}

			if (testSrcLen >= std::min(srcLen, CharInputConverter::MAX_INPUT_CHAR_BYTES)) {
				reportError(src, testSrcLen);
				return std::make_pair(testSrcLen, errorChar_);
			}
		}
	}

private:

	// Try to convert a single source character of length srcLen. The character
	// must be at least this long (i.e. all srcLen input character must be
	// required) but may require more characters.
	// If the character is the correct length and valid then returns the character
	// and true.
	// If the character is the correct length but invalid then returns the EOF
	// character and true.
	// If the source is just part of a longer character then returns  the EOF
	// character and false.
	std::pair<Char, bool> tryConvertChar(const char* src, Uint srcLen) {
		ASSERT(srcLen > 0u);

		// At most 2 output words per input character.
		wchar_t wbuf[3];
		memset(wbuf, 0, sizeof(wbuf));
		wchar_t* pwbuf = 0;
		const char* psrc = 0;

		// For safety we always flush before a conversion
		mbstate_ = std::mbstate_t();

		// Run the conversion
		// Note that this conversion seems to give the same result as using
		// MultiByteToWideChar(CP_ACP, ...) under Windows.
		CodecvtType::result codecvtResult = facet_.in(
			mbstate_,
			src, &src[srcLen], psrc,
			wbuf, wbuf + 2, pwbuf);
		Uint srcReadLen = (Uint)(psrc - src);
		Uint dstWriteLen = (Uint)(pwbuf - wbuf);

		// Must have read all the source input. This is a precondition.
		ASSERT(srcReadLen == srcLen);

		if (codecvtResult == CodecvtType::partial) {
			// Insufficient source characters
			return std::make_pair(Char::eof(), false);
		}
		else if (codecvtResult == CodecvtType::ok) {
			// Success
#if BUILD(WINDOWS)
			ASSERT(sizeof(wchar_t) == 2u);
			ASSERT((dstWriteLen >= 1u) && (dstWriteLen <= 2u));
			Uint len = 0u;
			Char ch = Char::fromUtf16((const char16_t*)wbuf, len);
			ASSERT(len == dstWriteLen);
#elif BUILD(LINUX)
			ASSERT(sizeof(wchar_t) == 4u);
			ASSERT(dstWriteLen == 1u);
			Char ch = Char::fromUtf32((char32_t)*wbuf);
#else
#error
#endif
			return std::make_pair(ch, true);
		}
		else {
			// Not allowed noconv so must be an error
			ASSERT(codecvtResult == CodecvtType::error);
			return std::make_pair(Char::eof(), true);
		}
	}

private:
	// The default locale. Note std::locale("") is not the same as std::locale().
	// The former is what we use for ANSI and is the default operating system
	// locale. The latter is the "C" locale.
	std::locale defaultLocale_;

	// The code converter.
	typedef std::codecvt<wchar_t, char, std::mbstate_t> CodecvtType;
	const CodecvtType& facet_;

	// The internal code converter state.
	std::mbstate_t mbstate_;

	// The size of the map entries. All values for one byte.
	static const Uint MAP_SIZE = 256u;

	// True to use the corresponding entry in map_.
	bool useMap_[MAP_SIZE];		

	// A character map for single byte input characters.
	Char map_[MAP_SIZE];			
};
