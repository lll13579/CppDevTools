#pragma once
#include "Util/Assert.hpp"
#include "Util/OutputStream.hpp"

// Extra stream objects for the OutputStreamWithIndent.
// sindent increases indentation by 1
// sundent decreases indentation by 1
enum SindentType { sindent };
enum SundentType { sundent };

// The output stream with indent class supports automatic indentation control.
// Any text output after a \n is indented with tabs to the current indentation
// level. An empty line is not indented.
class OutputStreamWithIndent : public OutputStream {
public:
	// Construct from another output stream which will receive the
	// indented text.
	OutputStreamWithIndent(OutputStream& os) :
		OutputStream(),
		os_(os),
		indentLevel_(0),
		atStartOfLine_(true)
	{
	}
	~OutputStreamWithIndent() {
	}
	void streamChar(Char ch) {
		if (ch == Char('\n')) {
			atStartOfLine_ = true;
		}
		else if (atStartOfLine_) {
			for (Uint i = 0; i < indentLevel_; ++i) {
				os_.streamChar(Char('\t'));
			}
			atStartOfLine_ = false;
		}
		os_.streamChar(ch);
	}
	void increaseIndent() {
		++indentLevel_;
	}
	void decreaseIndent() {
		ASSERT(indentLevel_ > 0);
		--indentLevel_;
	}
	void clearIndent() {
		indentLevel_ = 0;
	}
private:
	template <typename T> friend OutputStreamWithIndent& operator<<(OutputStreamWithIndent& os, const T& x);
	OutputStream& os_;
	Uint indentLevel_;
	bool atStartOfLine_;
};

// Everything normal is streamed by the underlying OutputStream. Note we cannot avoid this
// call by directly using the OutputStream base class because that would return a
// OutputStream& and chained operation such as fout << "hello" << sindent would output
// an int for the sindent rather than change the indent level.
template <typename T> OutputStreamWithIndent& operator<<(OutputStreamWithIndent& os, const T& x) {
	*dynamic_cast<OutputStream*>(&os) << x;
	return os;
}

// os << sindent
inline OutputStreamWithIndent& operator<<(OutputStreamWithIndent& os, const SindentType&) {
	os.increaseIndent();
	return os;
}

// os << sundent
inline OutputStreamWithIndent& operator<<(OutputStreamWithIndent& os, const SundentType&) {
	os.decreaseIndent();
	return os;
}

