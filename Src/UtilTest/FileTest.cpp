#include "TestTool/TestFile.hpp"
#include "TestTool/TestUtil.hpp"
#include "Util/Char.hpp"
#include "Util/CharEncoding.hpp"
#include "Util/SystemCout.hpp"
#include "Util/String.hpp"

// Test basic operations of the file system
AUTO_TEST_CASE {
	DirPath sidBaseDir = TestFile::getSrcDir("");
	scout << "Source input base dir is " << sidBaseDir.str() << sendl;
	CHECK(sidBaseDir.exists());

	DirPath sidUtilDir = TestFile::getSrcDir("UtilTest");
	scout << "Source input UtilTest dir is " << sidUtilDir.str() << sendl;
	CHECK(sidUtilDir.exists());

	DirPath sidUtilResourceDir = TestFile::getSrcDir("UtilTest/Resource");
	scout << "Source input UtilTest/Resource dir is " << sidUtilResourceDir.str() << sendl;
	CHECK(sidUtilResourceDir.exists());

	FilePath sifThis = TestFile::getSrcFile("UtilTest/FileTest.cpp");
	scout << "This file is " << sifThis.str() << sendl;
	CHECK(sifThis.exists());
	CHECK(sifThis.str().caselessEquals(__FILE__));
	CHECK(sifThis.getParentDir().str() == sidUtilDir.str());
	CHECK(sifThis.getLeafName() == String("FileTest.cpp"));
	FilePath sifTestTxt = TestFile::getSrcFile("UtilTest/Resource/FileTest.txt");
	scout << "Test text file is " << sifTestTxt.str() << sendl;
	CHECK(sifTestTxt.exists());
	CHECK(sifTestTxt.getParentDir().str() == sidUtilResourceDir.str());
	CHECK(sifTestTxt.getLeafName() == String("FileTest.txt"));
	CHECK(sifTestTxt.getParentDir().getLeafName() == String("Resource"));
	CHECK(sifTestTxt.getParentDir().getParentDir().getLeafName() == String("UtilTest"));

	DirPath bodBaseDir = TestFile::getBuildDir("");
	scout << "Build output base dir is " << bodBaseDir.str() << sendl;

	DirPath bodUtilDir = TestFile::getBuildDir("UtilTest");
	scout << "Build output UtilTest dir is " << bodUtilDir.str() << sendl;

	FilePath bodUtilTestObjFile = TestFile::getBuildFile("UtilTest/FileTest.obj");
	scout << "Build output UtilTest/FileTest.obj file is " << bodUtilTestObjFile.str() << sendl;

	DirPath todBaseDir = TestFile::getTestDir("");
	scout << "Test output base dir is " << todBaseDir.str() << sendl;

	DirPath todUtilDir = TestFile::getTestDir("UtilTest");
	scout << "Test output UtilTest dir is " << todUtilDir.str() << sendl;

	DirPath todUtilFileDir = TestFile::getTestDir("UtilTest/File");
	scout << "Test output UtilTest/File dir is " << todUtilFileDir.str() << sendl;

	DirPath todUtilFileADir = TestFile::getTestDir("UtilTest/File/A");
	scout << "Test output UtilTest/File/A dir is " << todUtilFileADir.str() << sendl;

	DirPath todUtilFileBDir = TestFile::getTestDir("UtilTest/File/B");
	scout << "Test output UtilTest/File/B dir is " << todUtilFileBDir.str() << sendl;

	FilePath todUtilFileBbFile = TestFile::getTestFile("UtilTest/File/B/b.txt");
	scout << "Test output UtilTest/File/B/b.txt file is " << todUtilFileBbFile.str() << sendl;

	TestFile::createTestDir(todUtilFileADir);
	std::string fileBContents = "Contents for file B 123456789";
	TestFile::createBinaryTestFile(todUtilFileBbFile, fileBContents);

	// Check existence
	CHECK(bodUtilDir.exists());
	CHECK(bodUtilTestObjFile.exists());
	CHECK(todUtilFileADir.exists());
	CHECK(todUtilFileDir.exists());
	CHECK(todUtilDir.exists());
	CHECK(todUtilFileBDir.exists());
	CHECK(todUtilFileBbFile.exists());
	CHECK(todUtilFileBbFile.getParentDir().str() == todUtilFileBDir.str());

	// Open for input binary test file
	{
		FileBinaryInputPtr testTxtFile = FileBinaryInput::open(sifTestTxt);
		std::string filePart1 = "ABCD-TEST,";
		std::string filePart2 = "EFGHIJKL-TEST,";
		std::string filePart3 = "MNOPQR-TEST";
		const Uint BUFFER_SIZE = 100;
		char buffer[BUFFER_SIZE];
		// Read line 1
		Uint result = testTxtFile->read(buffer, (Uint)filePart1.size());
		CHECK(result == filePart1.size());
		CHECK(std::string((const char*)buffer, filePart1.size()) == filePart1);
		// Read line 2
		result = testTxtFile->read(buffer, (Uint)filePart2.size());
		CHECK(result == filePart2.size());
		CHECK(std::string((const char*)buffer, filePart2.size()) == filePart2);
		// Read line 3
		result = testTxtFile->read(buffer, BUFFER_SIZE);
		CHECK(result == filePart3.size());
		CHECK(std::string((const char*)buffer, filePart3.size()) == filePart3);
		// Try to read another line
		result = testTxtFile->read(buffer, BUFFER_SIZE);
		CHECK(result == 0);
		// And again
		result = testTxtFile->read(buffer, BUFFER_SIZE);
		CHECK(result == 0);
	}

	// Open for input file B
	{
		FileBinaryInputPtr fileB = FileBinaryInput::open(todUtilFileBbFile);
		const Uint BUFFER_SIZE = 100;
		char buffer[BUFFER_SIZE];
		// Read
		Uint result = fileB->read(buffer, BUFFER_SIZE);
		CHECK(result == fileBContents.size());
	}

	// Open for output file B
	{
		FileBinaryOutputPtr fileBOut = FileBinaryOutput::create(todUtilFileBbFile);
		std::string newContents1 = "new contents";
		std::string newContents2 = "abcd";
		fileBOut->write(newContents1.c_str(), (Uint)newContents1.size());
		fileBOut->write(newContents2.c_str(), (Uint)newContents2.size());
		fileBOut->close();
		// Read it back
		FileBinaryInputPtr fileBIn = FileBinaryInput::open(todUtilFileBbFile);
		CHECK(fileBIn->readString() == newContents1 + newContents2);
	}

	// File system tests
	DirPath workDir = FileSystem::getWorkingDir();
	scout << "Working dir is " << workDir.str() << sendl;
	CHECK(workDir.isValid());
	CHECK(workDir.exists());
	// Test FilePath
	FilePath fileBDuplicate1(todUtilFileBbFile.str());
	FilePath fileBDuplicate2(todUtilFileBbFile.str(), workDir);
	FilePath fileBDuplicate3("File/B/b.txt", todUtilDir);
	CHECK(fileBDuplicate1.str() == todUtilFileBbFile.str());
	CHECK(fileBDuplicate2.str() == todUtilFileBbFile.str());
	CHECK(fileBDuplicate3.str() == todUtilFileBbFile.str());
	// Test DirPath
	DirPath dirADuplicate1(todUtilFileADir.str());
	DirPath dirADuplicate2(todUtilFileADir.str(), workDir);
	DirPath dirADuplicate3("File/A", todUtilDir);
	CHECK(dirADuplicate1.str() == todUtilFileADir.str());
	CHECK(dirADuplicate2.str() == todUtilFileADir.str());
	CHECK(dirADuplicate3.str() == todUtilFileADir.str());
}

// Test locale encoded files 
AUTO_TEST_CASE {
	// Write binary, read locale

	// UTF8 sequence
	// \xc2\xa2         is U+00a2
	// \xe2\x82\xac     is U+20ac
	// \xf0\xa4\xad\xa2 is U+24b62
	String special = "\xc2\xa2\xe2\x82\xac\xf0\xa4\xad\xa2";
	String filename = String("UtilTest/File/locale") + special + "end/file" + special + "end.txt";
	FilePath filePath = TestFile::getTestFile(filename);
#if 0
	// These do not work with Unicode yet. But they are only used for testing so
	// low priority.
	CHECK(filePath.getLeafName() == String("file") + special + "end.txt");
	CHECK(filePath.getParentDir().getLeafName() == String("locale") + special + "end");
#endif
	CHECK(filePath.getParentDir().getParentDir().getLeafName() == String("File"));

	std::string fileContents = "Contents for locale file\r\n";
	// 0x80 is euro symbol U+20ac
	// 0x86 is dagger U+2020
	// 0xa3 is pound U+00a3
	// 0xf7 is divide U+00f7
	fileContents += "Hello \x80\x86\xa3\xf7 World\r\n";
	fileContents += "End";
	TestFile::createBinaryTestFile(filePath, fileContents);
	CHECK(filePath.exists());

	FileEncodedInputPtr in = FileEncodedInput::open(CharEncoding::DEFAULT, filePath);
	String s = in->readString(24);
	CHECK(s == "Contents for locale file");
	CHECK(in->getLine() == 1);
	CHECK(in->read() == Char('\n'));
	CHECK(in->getLine() == 1);
	CHECK(in->read() == Char('H'));
	CHECK(in->getLine() == 2);
	s = in->readString(5);
	CHECK(s == "ello ");
	CHECK(in->read() == Char::fromUtf32(0x20ac));
	CHECK(in->read() == Char::fromUtf32(0x2020));
	CHECK(in->read() == Char::fromUtf32(0x00a3));
	CHECK(in->read() == Char::fromUtf32(0x00f7));
	s = in->readString();
	CHECK(s == " World\nEnd");
	CHECK(in->getLine() == 3);
}

// Test basic operations of the virtual file system
AUTO_TEST_CASE {
	FileSystem::startVirtualFileSystem();

	TestFile::createTestDir("UtilTest/brave/new");
	FilePath bnw = TestFile::getTestFile("UtilTest/brave/new/$world.txt");
	{
		FileEncodedOutputPtr out = FileEncodedOutput::create(CharEncoding::DEFAULT, bnw);
		out->write(String("Oh brave new"));
		out->write(String(" world\nThat"));
		out->write(String(" has such people in it\n"));
		out->close();
	}

	CHECK(bnw.exists());
	CHECK(bnw.getLeafName() == String("$world.txt"));
	CHECK(bnw.getParentDir().getLeafName() == String("new"));

	{
		FileEncodedInputPtr in = FileEncodedInput::open(CharEncoding::DEFAULT, bnw);
		CHECK(in->getLine() == 1);
		CHECK(in->read() == Char('O'));
		CHECK(in->read() == Char('h'));
		CHECK(in->readString(16u) == String(" brave new world"));
		CHECK(in->getLine() == 1);
		CHECK(in->read() == Char('\n'));
		CHECK(in->getLine() == 1);
		CHECK(in->read() == Char('T'));
		CHECK(in->getLine() == 2);
		CHECK(in->readString() == String("hat has such people in it\n"));
		CHECK(in->getLine() == 3);
		CHECK(in->read() == Char::eof());
		CHECK(in->read() == Char::eof());
	}

	FileSystem::stopVirtualFileSystem();
}
