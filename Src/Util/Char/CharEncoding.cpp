#include <cwchar>        // std::mbstate_t
#include <iomanip>
#include <locale>
#include <sstream>
#include "Util/Assert.hpp"
#include "Util/CharEncoding.hpp"
#include "Util/String.hpp"

CharEncoding::CharEncoding(const String& name) :
	ordinal_(INVALID) 
{
#if BUILD(WINDOWS)
	     if (name == "ansi")	ordinal_ = ANSI;
	else if (name == "oem")		ordinal_ = OEM;
	else
#endif
		 if (name == "utf8")	ordinal_ = UTF8;
	else if (name == "utf16be") ordinal_ = UTF16BE;
	else if (name == "utf16le") ordinal_ = UTF16LE;
	else if (name == "utf32be")	ordinal_ = UTF32BE;
	else if (name == "utf32le")	ordinal_ = UTF32LE;
	else if (name == "default") ordinal_ = DEFAULT;
	else						ordinal_ = INVALID;
}

CharEncoding::CharEncoding(Ordinal ordinal) :
	ordinal_(ordinal)
{
}

bool CharEncoding::isValid() const {
	return ordinal_ != INVALID;
}

String CharEncoding::getName() const {
	switch (ordinal_) {
#if BUILD(WINDOWS)
	case ANSI:	  return "ansi";
	case OEM:	  return "oem";
#endif
	case UTF8:	  return "utf8";
	case UTF16BE: return "utf16be";
	case UTF16LE: return "utf16le";
	case UTF32BE: return "utf32be";
	case UTF32LE: return "utf32le";
	default:
		FAIL;
	}
}
