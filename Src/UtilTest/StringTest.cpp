#include "Util/String.hpp"
#include "TestTool/TestUtil.hpp"

AUTO_TEST_CASE {
	String zero;
	CHECK(zero.empty());

	String fred = "fred bloggs";
	String X = 'X';
	CHECK(X == "X");
	
	StringIter pos1 = fred.begin();
	StringIter pos2 = fred.end();
	CHECK(*pos2 == Char::eof());
	++pos1;
	pos1++;
	--pos2;
	pos2--;
	String middle(pos1, pos2);
	CHECK(middle == "ed blog");
	middle.clear();
	CHECK(middle == "");

	String s = X;
	s += " and ";
	s += fred;
	s += Char('Z');
	CHECK(s == "X and fred bloggsZ");

	String t = X + fred + Char('Y') + "1234";
	CHECK(t == "Xfred bloggsY1234");

	CHECK(fred.front() == Char('f'));
	CHECK(fred.back() == Char('s'));
	String empty;
	CHECK(empty.front() == Char::eof());
	CHECK(empty.back() == Char::eof());
}

AUTO_TEST_CASE {
	String s = "HelloAbc123WoWldAbc123";

	String abc = "Abc";
	StringIterPair firstAbc = s.findFirst(abc);
	StringIterPair first123 = s.findFirst("123");
	StringIter firstW = s.findFirst(Char('W'));
	StringIterPair absent = s.findFirst("4");

	CHECK(!firstAbc.atEnd());
	CHECK(absent.atEnd());
	CHECK(firstAbc.first().substrBefore() == "Hello");
	CHECK(firstAbc.second() == first123.first());
	CHECK(first123.second() == firstW);
	CHECK(first123.second().substrAfter() == "WoWldAbc123");

	StringIterPair secondAbc = firstAbc.second().findNext(abc);
	StringIterPair second123 = first123.second().findNext("123");
	CHECK(secondAbc.first().substrAfter() == "Abc123");
	CHECK(secondAbc.second() == second123.first());
	CHECK(second123.second() == s.end());

	CHECK(!second123.first().atBegin());
	CHECK(!second123.first().atEnd());
	CHECK(second123.second().atEnd());
	CHECK(s.begin().atBegin());

	StringIterPair lastAbc = s.findLast(abc);
	StringIterPair last123 = s.findLast("123");
	StringIter lastW = s.findLast(Char('W'));
	StringIterPair absent2 = s.findLast("4");

	CHECK(!lastAbc.atEnd());
	CHECK(absent2.atEnd());
	CHECK(lastAbc.first().substrBefore() == "HelloAbc123WoWld");
	CHECK(lastAbc.second().substrAfter() == "123");
	CHECK(last123.first().substrBefore() == "HelloAbc123WoWldAbc");
	CHECK(last123.second().atEnd());
	CHECK(lastW.substrAfter() == "WldAbc123");
}

AUTO_TEST_CASE {
	String s = "1234567";
	StringIter p1 = s.begin();
	CHECK(*p1++ == Char('1'));
	CHECK(*++p1 == Char('3'));
	StringIter p2 = s.end();
	CHECK(*--p2 == Char('7'));
	CHECK(*p2-- == Char('7'));
	CHECK(*p2 == Char('6'));
	p1++;
	p1++;
	CHECK(p1 < p2);
	p2--;
	CHECK(p1 == p2);
	p1++;
	CHECK(p1 > p2);
}

AUTO_TEST_CASE {
	// \xc2\xa2         is U+00a2
	// \xf0\xa4\xad\xa2 is U+24b62
	String s = "PQ\xc2\xa2RS\xf0\xa4\xad\xa2TU";
	StringIter p = s.findFirst(Char::fromUtf32(0xa2u));
	CHECK(*p == Char::fromUtf32(0xa2u));
	StringIter p2 = ++p;
	CHECK(*p == 'R');
	CHECK(*p2 == 'R');
	p2++; p2++;
	CHECK(*p2 == Char::fromUtf32(0x24b62u));
	StringIter p3 = --p2;
	CHECK(*p2 == 'S');
	CHECK(*p3 == 'S');
	p3--; p3--;
	CHECK(*p3 == Char::fromUtf32(0xa2u));
	CHECK(s.toUtf8() == "PQ\xc2\xa2RS\xf0\xa4\xad\xa2TU");
}

AUTO_TEST_CASE {
	CHECK(String("AaZz09_%").toUpperCopy() == "AAZZ09_%");
	CHECK(String("AaZz09_%").toLowerCopy() == "aazz09_%");
	CHECK(String("AAaaZZzz09_%").caselessEquals(String("AaAaZzZz09_%")));
	CHECK(!String("AAaaZZzz09_%x").caselessEquals(String("AaAaZzZz09_%")));
	CHECK(!String("AAaxZZzz09_%").caselessEquals(String("AaAaZzZz09_%")));

	CHECK(String("hello world").trimCopy() == "hello world");
	CHECK(String(" \t\n\v\f hello \t\n\v\f ").trimCopy() == "hello");
}

AUTO_TEST_CASE {
	CHECK(String("AaZz09_%").beginsWith("AaZz"));
	CHECK(!String("AaZz09_%").beginsWith("AAZz"));
	CHECK(String("AaZz09_%").beginsWith("AaZz09_%"));
	CHECK(!String("AaZz09_%").beginsWith("AaZz09_% "));
	CHECK(String("AaZz09_%").beginsWith(""));
	CHECK(String("AaZz09_%").endsWith("09_%"));
	CHECK(!String("AaZz09_%").endsWith("08_%"));
	CHECK(String("AaZz09_%").endsWith("AaZz09_%"));
	CHECK(!String("AaZz09_%").endsWith(" AaZz09_%"));
	CHECK(String("AaZz09_%").endsWith(""));
	CHECK(String("AaZz09_%").caselessBeginsWith("AaZz"));
	CHECK(String("AaZz09_%").caselessBeginsWith("AAZz"));
	CHECK(String("AaZz09_%").caselessBeginsWith("AaZz09_%"));
	CHECK(String("AaZz09_%").caselessBeginsWith("AAzz09_%"));
	CHECK(!String("AaZz09_%").caselessBeginsWith("AaZz09_% "));
	CHECK(String("AaZz09_%").caselessBeginsWith(""));
}
