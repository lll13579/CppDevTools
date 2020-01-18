# CppDevTools
Tools for C++ development on Windows.

A collection of tools which may helpful for certain C++ projects. See the source files for documentation.
We are unable to provide any further assistance.

To build load Src/all.sln in Microsoft Visual Studio and build all. There are no prerequisites.

# AssertHashMap
A tool for creating a map from an assertion error identifier such as 0073-837881 to its location such as Util\Impl\String.cpp(52).
The benefit is that file paths and line numbers for the ASSERT and FAIL macros are not compiled into the object files for each
assertion error but the location of an error can subsequently be recovered by looking it up in the (private) AssertHashMap output.
So it keeps the file naming scheme confidential. Run without arguments - the output is in Test/.../AssertHashMap/_HashMap.txt.

# CheckLineEndings
Simply checks that all source files have Windows line endings. Run without arguments. No output is good!

# Make
A collection of make-related files. Executable does nothing.

# MakeGen
A generator application which creates a bath file which can be run to compile or post-process the defined projects.
Nowadays something like CMake is a better alternative but still useful for customising your own post processing
tools. Run with either -inc or -full arguments for incremental or full build. The output batch file is in
Build/.../MakeGen/MakeScript.bat.

# TestTool
A library of tools for testing. See TestToolTest and UtilTest for examples of how to use. Run without arguments and check for no errors.

# TestToolTest
Tests for TestTool.

# Util
A library of utility classes and functions. Char and String are character and string classes which enforce Unicode storage.
Also file and streaming utilities.

# UtilTest
Tests for Util. Run without arguments and check for no errors.

