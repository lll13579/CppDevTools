#include "Util/File.hpp"
#include "Util/File/FileSystemPlatform.hpp"
#include "Util/File/FileSystemVirtual.hpp"

namespace FileSystem {

	DirPath getWorkingDir() {
		return FileSystemPlatform::instance()->getWorkingDir();
	}

	void startVirtualFileSystem() {
		FileSystemVirtual::instance()->setEnable(true);
	}

	void stopVirtualFileSystem() {
		FileSystemVirtual::instance()->setEnable(false);
	}

	const String& getPathSeparator() {
#if BUILD(WINDOWS)
		static const String ret = "\\";
#elif BUILD(LINUX)
		static const String ret = "/";
#else
#error "Illegal build"
#endif
		return ret;
	}

	const String& getEscapedPathSeparator() {
#if BUILD(WINDOWS)
		static const String ret = "\\\\";
#elif BUILD(LINUX)
		static const String ret = "/";
#else
#error "Illegal build"
#endif
		return ret;
	}

}
