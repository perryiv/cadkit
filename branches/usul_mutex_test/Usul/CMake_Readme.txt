The "CMakeLists.txt" file can be used to generate a native build system set of 
files for your platform.

Go to www.cmake.org and download and compile cmake 2.4.2 (current as of July 7,

 2006). Once you have compiled and installed CMake you can now use CMake to 
 create the Makefile used to build Helios. The CMake build is setup to place 
 binaries in a folder called "Bin" which resides at the same level as Usul. But
 CMake also needs to place lots of temp files and intermediate build files some-
 where. The way that I use ( which works with Eclipse CDT ) is to create a 
 Folder called "Build" _within_ the Usul Directory. Then "cd" into this directory
 and type "ccmake ../". This will launch the curses based cmake configuration.
 Fill in the type of build (Debug or Release) and make sure that all the 
 libraries and Paths are correctly found. Hit the "c" key to 'configure' and then
 the "g" key to 'generate' the "makefile". Now type "make" and Usul should build
 and place the final dylib in the Bin/${Build Type} directory.
 
 Repeat the following process for all the other libs.
 
 Cheers
 