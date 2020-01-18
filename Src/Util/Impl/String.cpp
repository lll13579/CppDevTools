#include "Util/Assert.hpp"
#include "Util/Char.hpp"
#include "Util/CharOutputConverter.hpp"
#include "Util/String.hpp"

///////////////////////////////////////////////////////////////////////////////
// StringIter
///////////////////////////////////////////////////////////////////////////////

StringIter::StringIter(const StringIter& other) :
	s_(other.s_),
	pos_(other.pos_),
	ch_(other.ch_),
	len_(other.len_)
{	
}

StringIter& StringIter::operator=(const StringIter& other) {
	s_ = other.s_;
	pos_ = other.pos_;
	ch_ = other.ch_;
	len_ = other.len_;
	return *this;
}

const Char* StringIter::operator->() const {
	ASSERT(!ch_.isEof());
	return &ch_;
}

bool StringIter::operator==(const StringIter& other) const {
	ASSERT(s_ == other.s_);
	return pos_ == other.pos_;
}

bool StringIter::operator!=(const StringIter& other) const {
	ASSERT(s_ == other.s_);
	return pos_ != other.pos_;
}

bool StringIter::operator<(const StringIter& other) const {
	ASSERT(s_ == other.s_);
	return pos_ < other.pos_;
}

bool StringIter::operator<=(const StringIter& other) const {
	ASSERT(s_ == other.s_);
	return pos_ <= other.pos_;
}

bool StringIter::operator>(const StringIter& other) const {
	ASSERT(s_ == other.s_);
	return pos_ > other.pos_;
}

bool StringIter::operator>=(const StringIter& other) const {
	ASSERT(s_ == other.s_);
	return pos_ >= other.pos_;
}

StringIter StringIter::operator++() {
	pos_ += len_;
	init();
	return *this;
}

StringIter StringIter::operator++(int) {
	StringIter ret(*this);
	++(*this);
	return ret;
}

StringIter StringIter::operator--() {
	ASSERT(pos_ > 0);
	for (;;) {
		pos_--;
		unsigned char ch = (unsigned char)s_->str_[pos_];
		if ((ch < 0x80) || (ch >= 0xc0)) {
			break;
		}
		ASSERT(pos_ > 0);
	}
	init();
	return *this;
}

StringIter StringIter::operator--(int) {
	StringIter ret(*this);
	--(*this);
	return ret;
}

bool StringIter::atBegin() const {
	return pos_ == 0;
}

bool StringIter::atEnd() const {
	return ch_.isEof();
}

StringIterPair StringIter::findNext(const String& str) const {
	std::string::size_type pos = s_->str_.find(str.str_, pos_);
	if (pos == std::string::npos) {
		return StringIterPair(s_->end(), s_->end());
	}
	else {
		return StringIterPair(StringIter(*s_, (Uint)pos),
							 StringIter(*s_, (Uint)pos + (Uint)str.str_.size()));
	}
}

StringIter StringIter::findNext(Char ch) const {
	char utf8[5];
	Uint utf8Len = 0;
	utf8Len = ch.toUtf8(utf8);
	ASSERT(utf8Len <= 4);
	utf8[utf8Len] = '\0';

	std::string::size_type pos = s_->str_.find(utf8, pos_);
	if (pos == std::string::npos) {
		return s_->end();
	}
	else {
		return StringIter(*s_, (Uint)pos);
	}
}

String StringIter::substrBefore() const {
	return String(s_->str_.substr(0, pos_));
}

String StringIter::substrAfter() const {
	return String(s_->str_.substr(pos_));
}

StringIter::StringIter(const String& s, Uint pos) :
	s_(&s), 
	pos_(pos),
	ch_(Char::eof()),
	len_(0)
{
	init();
}

void StringIter::init() {
	ASSERT(pos_ <= s_->str_.size());
	if (pos_ == s_->str_.size()) {
		ch_ = Char::eof();
		len_ = 0;
	}
	else {
		len_ = 0;
		ch_ = Char::fromUtf8(s_->str_.c_str() + pos_, len_);
		ASSERT(!ch_.isEof());
		ASSERT(len_ > 0);
		ASSERT(pos_ + len_ <= s_->str_.size());
	}
}

///////////////////////////////////////////////////////////////////////////////
// StringIterPair
///////////////////////////////////////////////////////////////////////////////

StringIterPair::StringIterPair(const StringIter& first__, const StringIter& second__) :
	first_(first__), 
	second_(second__) 
{
	ASSERT(first_ <= second_);
}

///////////////////////////////////////////////////////////////////////////////
// String
///////////////////////////////////////////////////////////////////////////////

String::String(const StringIter& from, const StringIter& to) {
	ASSERT(from.s_ == to.s_);
	ASSERT(from.pos_ <= to.pos_);
	str_ = from.s_->str_.substr(from.pos_, to.pos_ - from.pos_);
}

Char String::back() const {
	if (str_.empty()) {
		return Char::eof();
	}
	else {
		return * -- end();
	}
}

String& String::operator+=(Char ch) {
	ASSERT(ch != Char::eof());
	if (ch.isAscii()) {
		str_ += (char)ch.toUtf32();
	}
	else {
		char utf8[5];
		Uint utf8Len = 0;
		utf8Len = ch.toUtf8(utf8);
		ASSERT(utf8Len <= 4);
		utf8[utf8Len] = '\0';
		str_ += utf8;
	}
	return *this;
}

StringIterPair String::findFirst(const String& str) const {
	std::string::size_type pos = str_.find(str.str_);
	if (pos == std::string::npos) {
		return StringIterPair(end(), end());
	}
	else {
		return StringIterPair(StringIter(*this, (Uint)pos),
							 StringIter(*this, (Uint)pos + (Uint)str.str_.size()));
	}
}

StringIter String::findFirst(Char ch) const {
	char utf8[5];
	Uint utf8Len = 0;
	utf8Len = ch.toUtf8(utf8);
	ASSERT(utf8Len <= 4);
	utf8[utf8Len] = '\0';

	std::string::size_type pos = str_.find(utf8);
	if (pos == std::string::npos) {
		return end();
	}
	else {
		return StringIter(*this, (Uint)pos);
	}
}

StringIterPair String::findLast(const String& str) const {
	std::string::size_type pos = str_.rfind(str.str_);
	if (pos == std::string::npos) {
		return StringIterPair(end(), end());
	}
	else {
		return StringIterPair(StringIter(*this, (Uint)pos),
							 StringIter(*this, (Uint)pos + (Uint)str.str_.size()));
	}
}

StringIter String::findLast(Char ch) const {
	char utf8[5];
	Uint utf8Len = 0;
	utf8Len = ch.toUtf8(utf8);
	ASSERT(utf8Len <= 4);
	utf8[utf8Len] = '\0';

	std::string::size_type pos = str_.rfind(utf8);
	if (pos == std::string::npos) {
		return end();
	}
	else {
		return StringIter(*this, (Uint)pos);
	}
}

#if BUILD(MSV)
PlatformString String::toPlatform() const {
	CharOutputConverterPtr cv = CharOutputConverter::create(CharEncoding::UTF16LE);
	std::string out = cv->convertString(*this);
	ASSERT(sizeof(wchar_t) == 2);
	ASSERT(out.size() % 2 == 0);
	return PlatformString((const wchar_t*)out.c_str(), out.size() / 2);
}
#else
PlatformString toPlatform() const {
	// The platform string is just UTF8.
	return str_;
}
#endif

String String::toUpperCopy() const {
	String ret;
	for (StringIter it = begin(); !it.atEnd(); ++it) {
		ret += it->toUpperCopy();
	}
	return ret;
}

String String::toLowerCopy() const {
	String ret;
	for (StringIter it = begin(); !it.atEnd(); ++it) {
		ret += it->toLowerCopy();
	}
	return ret;
}

bool String::caselessEquals(const String& other) const {
	if (str_.size() != other.str_.size()) {
		return false;
	}
	StringIter it1 = begin();
	StringIter it2 = other.begin();

	for (; !it1.atEnd() && !it2.atEnd(); ++it1, ++it2) {
		if ((*it1 != *it2) && (it1->toUpperCopy() != it2->toUpperCopy())) {
			return false;
		}
	}
	return it1.atEnd() && it2.atEnd();
}

bool String::beginsWith(const String& other) const {
	std::string::size_type thisSize = str_.size();
	std::string::size_type otherSize = other.str_.size();
	if (thisSize < otherSize) {
		return false;
	}
	return str_.substr(0, otherSize) == other.str_;
}

bool String::endsWith(const String& other) const {
	std::string::size_type thisSize = str_.size();
	std::string::size_type otherSize = other.str_.size();
	if (thisSize < otherSize) {
		return false;
	}
	return str_.substr(thisSize - otherSize, otherSize) == other.str_;
}

bool String::caselessBeginsWith(const String& other) const {
	StringIter it1 = begin();
	StringIter it2 = other.begin();

	for (; !it1.atEnd() && !it2.atEnd(); ++it1, ++it2) {
		if ((*it1 != *it2) && (it1->toUpperCopy() != it2->toUpperCopy())) {
			return false;
		}
	}
	return it2.atEnd();
}

String String::trimCopy() const {
	StringIter it1 = begin();
	StringIter it2 = end();
	for (; it1 != it2; ++it1) {
		if (!it1->isWhitespace()) {
			break;
		}
	}
	while (it1 != it2) {
		it2--;
		if (!it2->isWhitespace()) {
			++it2;
			break;
		}
	}
	return String(it1, it2);
}

void String::validate(const std::string& s) {
	// Need to allow for embedded null characters
	Uint remaining = (Uint)s.size();
	const char* p = s.c_str();
	while (remaining > 0) {
		Uint len = validate(p);
		ASSERT(len <= remaining);
		remaining -= len;
		p += len;
		if (remaining > 0) {
			ASSERT(*p == '\0');
			++p;
			remaining--;
		}
	}
}

Uint String::validate(const char* s) {
	Uint ret = 0;
	const char* p = s;
	while (*p != '\0') {
		Uint len = 0;
		Char ch = Char::fromUtf8(p, len);
		ASSERT(!ch.isEof());
		ASSERT(len > 0);
		ret += len;
		p += len;
	}
	return ret;
}
