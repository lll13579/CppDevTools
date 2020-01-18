#include <fstream>
#include <iostream>
#include "MakeGen/Def.hpp"
#include "MakeGen/Dir.hpp"
#include "MakeGen/File.hpp"
#include "MakeGen/Project.hpp"
#include "MakeGen/ProjectRegistry.hpp"
#include "MakeGen/ResourcePath.hpp"
#include "Util/Version.hpp"

// The arguments are:
// [1] "-inc" for an incremental build, "-full" for a full build.
int doMain(int argc, char** argv) {
	ASSERT(argc == 2);

	// Incremental build
	bool incrementalBuild = false;
	if (std::string(argv[1]) == "-inc") {
		incrementalBuild = true;
	}
	else {
		ASSERT(std::string(argv[1]) == "-full");
	}

	// The components of the output file.
	Dir outDir = Dir::BUILD("MakeGen");
	File outFile(outDir, MAKE_SCRIPT_STEM, MAKE_SCRIPT_EXT);
	fs::path outFilePath = outFile.getFsPath();

	// Ensure the output directory exists.
	outDir.create();

	// Rename the output file if it already exists. If it is a
	// full build then the build script will delete all these
	// files later anyway.
	if (fs::exists(outFilePath)) {
		ASSERT(fs::is_regular_file(outFilePath));
		File f = outFile;
		for (int i = 1; ; ++i) {
			f = File(outDir, String(MAKE_SCRIPT_STEM) << "_" << i, MAKE_SCRIPT_EXT);
			if (!fs::exists(f.getFsPath()))
			{
				break;
			}
		}
		outFile.rename(f);
		ASSERT(!fs::exists(outFilePath));
	}

	// Get the build order
	std::vector<Project*> buildOrder = ProjectRegistry::instance().buildOrder(incrementalBuild);

	// Open the output file.
	std::ofstream fout(outFilePath.string());

	// Create the script
	{
		Script script(fout);

		// Ensure that the Build and Test directories exist.
		script.createDir(Dir::BUILD());
		script.createDir(Dir::TEST());

		// Delete all files in the Build Make directory.
		// This includes the _EntireMakeSuccess.txt file.
		// There is no Test directory for Make.
		script.createDir(Dir::BUILD() / "Make");
		script.removeDirContents(Dir::BUILD() / "Make");

		// Output the options preamble
		script.compilerOptionsPreamble();
		script.resourceOptionsPreamble();
		script.linkerOptionsPreamble();

		// Output each project to be made.
		for (auto p : buildOrder) {
			p->make(script);
		}

		// Output the overall make success.
		script.blankLine();
		script.copyFile(
			File(Dir::SRC("MakeGen"), MAKE_SUCCESS_TXT),
			File(Dir::BUILD("Make"), "_EntireMakeSuccess.txt"));

		// Echo success
		script.blankLine();
		script.echo("****************************");
		script.echo("***** Build successful *****");
		script.echo("****************************");

		// Success
		script.successfulExit();
	}

	// Close the file
	fout.close();
	ASSERT(fout.good());

	// Success
	std::cout << std::endl;
	std::cout << "MakeGen executed successfully" << std::endl;
    return 0;
}

int main(int argc, char** argv) {
	try {
		int ret = doMain(argc, argv);
		return ret;
	}
	catch (AssertException&) {
		return 1;
	}
	catch (...) {
		return 1;
	}
}
