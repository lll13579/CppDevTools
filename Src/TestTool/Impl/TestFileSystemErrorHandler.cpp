#include <sstream>
#include "TestTool/TestFileSystemErrorHandler.hpp"
#include "TestTool/TestUtil.hpp"

FileSystemErrorHandlerPtr TestFileSystemErrorHandler::instance() {
	static FileSystemErrorHandlerPtr ret = std::make_shared<TestFileSystemErrorHandler>();
	return ret;
}

TestFileSystemErrorHandler::~TestFileSystemErrorHandler() { 
}

void TestFileSystemErrorHandler::cannotEncodeForInput(Line line, const std::string& hexSequence) {
	TestUtil::event(CannotEncodeForInputTestEvent::create(line, hexSequence));
}

void TestFileSystemErrorHandler::cannotEncodeForOutput(Char ch) {
	TestUtil::event(CannotEncodeForOutputTestEvent::create(ch));
}

void TestFileSystemErrorHandler::cannotOpenForRead(const FilePath& absFilePath) {
	TestUtil::event(CannotOpenForReadTestEvent::create(absFilePath));
}

void TestFileSystemErrorHandler::readError(const FilePath& absFilePath) {
	TestUtil::event(ReadErrorTestEvent::create(absFilePath));
}

void TestFileSystemErrorHandler::cannotOpenForWrite(const FilePath& absFilePath) {
	TestUtil::event(CannotOpenForWriteTestEvent::create(absFilePath));
}

void TestFileSystemErrorHandler::writeError(const FilePath& absFilePath) {
	TestUtil::event(WriteErrorTestEvent::create(absFilePath));
}

TestFileSystemErrorHandler::TestFileSystemErrorHandler() :
	FileSystemErrorHandler()
{
}

///////////////////////////////////////////////////////////////////////////////

TestEventPtr CannotEncodeForInputTestEvent::create(Line line, const std::string& hexSequence) {
	return std::make_shared<CannotEncodeForInputTestEvent>(line, hexSequence);
}

bool CannotEncodeForInputTestEvent::isIdenticalTo(TestEventPtr other) const {
	CannotEncodeForInputTestEvent* p = dynamic_cast<CannotEncodeForInputTestEvent*>(other.get());
	return p && (line_ == p->line_) && (hexSequence_ == p->hexSequence_);
}

String CannotEncodeForInputTestEvent::toString() const {
	std::ostringstream ss;
	ss << "File System: Cannot encode for input at line " << line_ << " sequence " << hexSequence_;
	return String(ss.str());
}

CannotEncodeForInputTestEvent::CannotEncodeForInputTestEvent(Line line, const std::string& hexSequence) :
	line_(line),
	hexSequence_(hexSequence)
{
}

///////////////////////////////////////////////////////////////////////////////

TestEventPtr CannotEncodeForOutputTestEvent::create(Char ch) {
	return std::make_shared<CannotEncodeForOutputTestEvent>(ch);
}

bool CannotEncodeForOutputTestEvent::isIdenticalTo(TestEventPtr other) const {
	CannotEncodeForOutputTestEvent* p = dynamic_cast<CannotEncodeForOutputTestEvent*>(other.get());
	return p && (ch_ == p->ch_);
}

String CannotEncodeForOutputTestEvent::toString() const {
	return String("File System: Cannot encode for output the character ") + String(ch_);
}

CannotEncodeForOutputTestEvent::CannotEncodeForOutputTestEvent(Char ch) :
	ch_(ch)
{
}

///////////////////////////////////////////////////////////////////////////////

TestEventPtr CannotOpenForReadTestEvent::create(const FilePath& absFilePath) {
	return std::make_shared<CannotOpenForReadTestEvent>(absFilePath);
}

bool CannotOpenForReadTestEvent::isIdenticalTo(TestEventPtr other) const {
	CannotOpenForReadTestEvent* p = dynamic_cast<CannotOpenForReadTestEvent*>(other.get());
	return p && (absFilePath_ == p->absFilePath_);
}

String CannotOpenForReadTestEvent::toString() const {
	return String("File System: Cannot open for read the file ") + absFilePath_.str();
}

CannotOpenForReadTestEvent::CannotOpenForReadTestEvent(const FilePath& absFilePath) :
	absFilePath_(absFilePath)
{
}

///////////////////////////////////////////////////////////////////////////////

TestEventPtr ReadErrorTestEvent::create(const FilePath& absFilePath) {
	return std::make_shared<ReadErrorTestEvent>(absFilePath);
}

bool ReadErrorTestEvent::isIdenticalTo(TestEventPtr other) const {
	ReadErrorTestEvent* p = dynamic_cast<ReadErrorTestEvent*>(other.get());
	return p && (absFilePath_ == p->absFilePath_);
}

String ReadErrorTestEvent::toString() const {
	return String("File System: Error reading file ") + absFilePath_.str();
}

ReadErrorTestEvent::ReadErrorTestEvent(const FilePath& absFilePath) :
	absFilePath_(absFilePath)
{
}

///////////////////////////////////////////////////////////////////////////////

TestEventPtr CannotOpenForWriteTestEvent::create(const FilePath& absFilePath) {
	return std::make_shared<CannotOpenForWriteTestEvent>(absFilePath);
}

bool CannotOpenForWriteTestEvent::isIdenticalTo(TestEventPtr other) const {
	CannotOpenForWriteTestEvent* p = dynamic_cast<CannotOpenForWriteTestEvent*>(other.get());
	return p && (absFilePath_ == p->absFilePath_);
}

String CannotOpenForWriteTestEvent::toString() const {
	return String("File System: Cannot open for write the file ") + absFilePath_.str();
}

CannotOpenForWriteTestEvent::CannotOpenForWriteTestEvent(const FilePath& absFilePath) :
	absFilePath_(absFilePath)
{
}

///////////////////////////////////////////////////////////////////////////////

TestEventPtr WriteErrorTestEvent::create(const FilePath& absFilePath) {
	return std::make_shared<WriteErrorTestEvent>(absFilePath);
}

bool WriteErrorTestEvent::isIdenticalTo(TestEventPtr other) const {
	WriteErrorTestEvent* p = dynamic_cast<WriteErrorTestEvent*>(other.get());
	return p && (absFilePath_ == p->absFilePath_);
}

String WriteErrorTestEvent::toString() const {
	return String("File System: Error writing file ") + absFilePath_.str();
}

WriteErrorTestEvent::WriteErrorTestEvent(const FilePath& absFilePath) :
	absFilePath_(absFilePath)
{
}
