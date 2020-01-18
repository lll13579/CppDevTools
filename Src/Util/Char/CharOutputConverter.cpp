#pragma once
#include "Util/Assert.hpp"
#include "Util/Char/LocaleCharOutputConverter.hpp"
#include "Util/Char/Utf16CharOutputConverter.hpp"
#include "Util/Char/Utf32CharOutputConverter.hpp"
#include "Util/Char/Utf8CharOutputConverter.hpp"
#include "Util/CharOutputConverter.hpp"
#include "Util/Def.hpp"

CharOutputConverterPtr CharOutputConverter::create(CharEncoding dstEncoding) {
	switch (dstEncoding.getOrdinal()) {
#if BUILD(WINDOWS)
	case CharEncoding::ANSI:
		return std::make_shared<LocaleCharOutputConverter>(true);
	case CharEncoding::OEM:
		return std::make_shared<LocaleCharOutputConverter>(false);
#elif BUILD(LINUX)
#error TODO
#else
#error
#endif
	case CharEncoding::UTF8:
		return std::make_shared<Utf8CharOutputConverter>();
	case CharEncoding::UTF16BE:
		return std::make_shared<Utf16CharOutputConverter>(true);
	case CharEncoding::UTF16LE:
		return std::make_shared<Utf16CharOutputConverter>(false);
	case CharEncoding::UTF32BE:
		return std::make_shared<Utf32CharOutputConverter>(true);
	case CharEncoding::UTF32LE:
		return std::make_shared<Utf32CharOutputConverter>(false);
	case CharEncoding::INVALID:
	default:
		FAIL;
	}
}

CharOutputConverter::~CharOutputConverter() {
}

bool CharOutputConverter::convertAppend(const String& src, std::string& dst) {
	bool ret = true;
	char buf[CharOutputConverter::MAX_OUTPUT_CHAR_BYTES];
	for (StringIter pos = src.begin(); !pos.atEnd(); ++pos) {
		Uint len = convertChar(*pos, buf);
		if (len == 0) {
			ret = false;
		}
		else {
			ASSERT(len <= CharOutputConverter::MAX_OUTPUT_CHAR_BYTES);
			dst.append(buf, len);
		}
	}
	return ret;
}

std::string CharOutputConverter::convertString(const String& src) {
	std::string ret;
	if (!convertAppend(src, ret)) {
		ret.clear();
	}
	return ret;
}

CharOutputConverter::CharOutputConverter(CharEncoding dstEncoding) :
	dstEncoding_(dstEncoding)
{
}
