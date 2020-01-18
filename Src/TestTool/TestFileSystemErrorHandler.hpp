#pragma once
#include "TestTool/TestEvent.hpp"
#include "Util/Def.hpp"
#include "Util/File.hpp"

// A file system error handler for test projects. An error is converted
// to a test event which can then be expected. Using this handler is
// optional.
class TestFileSystemErrorHandler : public FileSystemErrorHandler {
public:
	// Get an instance of this class (actually a singleton for efficiency)
	static FileSystemErrorHandlerPtr instance();

	// Destructor
	~TestFileSystemErrorHandler();

	// FileSystemErrorHandler virtual functions
	void cannotEncodeForInput(Line line, const std::string& hexSequence);
	void cannotEncodeForOutput(Char ch);
	void cannotOpenForRead(const FilePath& absFilePath);
	void readError(const FilePath& absFilePath);
	void cannotOpenForWrite(const FilePath& absFilePath);
	void writeError(const FilePath& absFilePath);

private:
	// Constructor
	TestFileSystemErrorHandler();
	ALLOW_MAKE_SHARED(TestFileSystemErrorHandler);
};

// Events corresponding to the above
class CannotEncodeForInputTestEvent : public TestEvent {
public:
	static TestEventPtr create(Line line, const std::string& hexSequence);
	bool isIdenticalTo(TestEventPtr other) const;
	String toString() const;
private:
	CannotEncodeForInputTestEvent(Line line, const std::string& hexSequence);
	ALLOW_MAKE_SHARED(CannotEncodeForInputTestEvent);
private:
	Line line_;
	std::string hexSequence_;
};

class CannotEncodeForOutputTestEvent : public TestEvent {
public:
	static TestEventPtr create(Char ch);
	bool isIdenticalTo(TestEventPtr other) const;
	String toString() const;
private:
	CannotEncodeForOutputTestEvent(Char ch);
	ALLOW_MAKE_SHARED(CannotEncodeForOutputTestEvent);
private:
	Char ch_;
};

class CannotOpenForReadTestEvent : public TestEvent {
public:
	static TestEventPtr create(const FilePath& absFilePath);
	bool isIdenticalTo(TestEventPtr other) const;
	String toString() const;
private:
	CannotOpenForReadTestEvent(const FilePath& absFilePath);
	ALLOW_MAKE_SHARED(CannotOpenForReadTestEvent);
private:
	FilePath absFilePath_;
};

class ReadErrorTestEvent : public TestEvent {
public:
	static TestEventPtr create(const FilePath& absFilePath);
	bool isIdenticalTo(TestEventPtr other) const;
	String toString() const;
private:
	ReadErrorTestEvent(const FilePath& absFilePath);
	ALLOW_MAKE_SHARED(ReadErrorTestEvent);
private:
	FilePath absFilePath_;
};

class CannotOpenForWriteTestEvent : public TestEvent {
public:
	static TestEventPtr create(const FilePath& absFilePath);
	bool isIdenticalTo(TestEventPtr other) const;
	String toString() const;
private:
	CannotOpenForWriteTestEvent(const FilePath& absFilePath);
	ALLOW_MAKE_SHARED(CannotOpenForWriteTestEvent);
private:
	FilePath absFilePath_;
};

class WriteErrorTestEvent : public TestEvent {
public:
	static TestEventPtr create(const FilePath& absFilePath);
	bool isIdenticalTo(TestEventPtr other) const;
	String toString() const;
private:
	WriteErrorTestEvent(const FilePath& absFilePath);
	ALLOW_MAKE_SHARED(WriteErrorTestEvent);
private:
	FilePath absFilePath_;
};
