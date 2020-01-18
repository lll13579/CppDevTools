#include <codecvt>
#include <cwchar>
#include <iomanip>
#include <locale>
#include <sstream>
#include "Util/Assert.hpp"
#include "Util/Char/LocaleCharInputConverter.hpp"
#include "Util/Char/Utf16CharInputConverter.hpp"
#include "Util/Char/Utf32CharInputConverter.hpp"
#include "Util/Char/Utf8CharInputConverter.hpp"
#include "Util/CharInputConverter.hpp"
#include "Util/Def.hpp"

CharInputConverterPtr CharInputConverter::create(
	CharEncoding srcEncoding, 
	CharInputConverterErrorHandlerPtr errorHandler, 
	Char errorChar)
{
	switch (srcEncoding.getOrdinal()) {
#if BUILD(WINDOWS)
	case CharEncoding::ANSI:
		return std::make_shared<LocaleCharInputConverter>(true, errorHandler, errorChar);
	case CharEncoding::OEM:
		return std::make_shared<LocaleCharInputConverter>(false, errorHandler, errorChar);
#elif BUILD(LINUX)
#error TODO
#else
#error
#endif
	case CharEncoding::UTF8:
		return std::make_shared<Utf8CharInputConverter>(errorHandler, errorChar);
	case CharEncoding::UTF16BE:
		return std::make_shared<Utf16CharInputConverter>(true, errorHandler, errorChar);
	case CharEncoding::UTF16LE:
		return std::make_shared<Utf16CharInputConverter>(false, errorHandler, errorChar);
	case CharEncoding::UTF32BE:
		return std::make_shared<Utf32CharInputConverter>(true, errorHandler, errorChar);
	case CharEncoding::UTF32LE:
		return std::make_shared<Utf32CharInputConverter>(false, errorHandler, errorChar);
	case CharEncoding::INVALID:
	default:
		FAIL;
	}
}

CharInputConverter::~CharInputConverter() {
}

void CharInputConverter::convertAppend(const char* src, Uint srcLen, String& dst) {
	const char* p = src;
	Uint remaining = srcLen;
	while (remaining > 0) {
		std::pair<Uint, Char> out = convertChar(p, remaining);
		ASSERT(out.first > 0);
		ASSERT(out.first <= remaining);
		p += out.first;
		remaining -= out.first;
		dst += out.second;
	}
}

void CharInputConverter::convertAppend(const std::string& src, String& dst) {
	convertAppend(src.c_str(), (Uint)src.size(), dst);
}

String CharInputConverter::convertString(const char* src, Uint srcLen) {
	String ret;
	convertAppend(src, srcLen, ret);
	return ret;
}

String CharInputConverter::convertString(const std::string& src) {
	String ret;
	convertAppend(src, ret);
	return ret;
}

CharInputConverter::CharInputConverter(
	CharEncoding srcEncoding, 
	CharInputConverterErrorHandlerPtr errorHandler, 
	Char errorChar)
	:
	srcEncoding_(srcEncoding),
	errorHandler_(errorHandler),
	errorChar_(errorChar)
{
}

void CharInputConverter::reportError(const char* src, Uint srcLen) const {
	if (errorHandler_) {
		std::stringstream ss;
		for (Uint i = 0; i < srcLen; i++) {
			if (i != 0) {
				ss << ' ';
			}
			ss << "0x" << std::hex << std::setfill('0') << std::setw(2) << (Uint)(Uint8)src[i];
		}
		errorHandler_->cannotEncodeForInput(ss.str());
	}
}
