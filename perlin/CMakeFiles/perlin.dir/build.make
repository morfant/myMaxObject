# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CMake.app/Contents/bin/cmake

# The command to remove a file.
RM = /Applications/CMake.app/Contents/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Users/giy/Documents/Max 8/Packages/max-sdk/source/my/perlin"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Users/giy/Documents/Max 8/Packages/max-sdk/source/my/perlin"

# Include any dependencies generated for this target.
include CMakeFiles/perlin.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/perlin.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/perlin.dir/flags.make

CMakeFiles/perlin.dir/perlin.o: CMakeFiles/perlin.dir/flags.make
CMakeFiles/perlin.dir/perlin.o: perlin.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/giy/Documents/Max 8/Packages/max-sdk/source/my/perlin/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/perlin.dir/perlin.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/perlin.dir/perlin.o -c "/Users/giy/Documents/Max 8/Packages/max-sdk/source/my/perlin/perlin.c"

CMakeFiles/perlin.dir/perlin.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/perlin.dir/perlin.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/Users/giy/Documents/Max 8/Packages/max-sdk/source/my/perlin/perlin.c" > CMakeFiles/perlin.dir/perlin.i

CMakeFiles/perlin.dir/perlin.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/perlin.dir/perlin.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/Users/giy/Documents/Max 8/Packages/max-sdk/source/my/perlin/perlin.c" -o CMakeFiles/perlin.dir/perlin.s

# Object files for target perlin
perlin_OBJECTS = \
"CMakeFiles/perlin.dir/perlin.o"

# External object files for target perlin
perlin_EXTERNAL_OBJECTS =

/Users/giy/Documents/Max\ 8/Packages/max-sdk/externals/perlin.mxo/Contents/MacOS/perlin: CMakeFiles/perlin.dir/perlin.o
/Users/giy/Documents/Max\ 8/Packages/max-sdk/externals/perlin.mxo/Contents/MacOS/perlin: CMakeFiles/perlin.dir/build.make
/Users/giy/Documents/Max\ 8/Packages/max-sdk/externals/perlin.mxo/Contents/MacOS/perlin: CMakeFiles/perlin.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/Users/giy/Documents/Max 8/Packages/max-sdk/source/my/perlin/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking C CFBundle shared module \"/Users/giy/Documents/Max 8/Packages/max-sdk/externals/perlin.mxo/Contents/MacOS/perlin\""
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/perlin.dir/link.txt --verbose=$(VERBOSE)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Copy PkgInfo"
	cp /Users/giy/Documents/Max\ 8/Packages/max-sdk/source/max-sdk-base/script/PkgInfo /Users/giy/Documents/Max\ 8/Packages/max-sdk/source/my/perlin/../../../externals/perlin.mxo/Contents/PkgInfo

# Rule to build all files generated by this target.
CMakeFiles/perlin.dir/build: /Users/giy/Documents/Max\ 8/Packages/max-sdk/externals/perlin.mxo/Contents/MacOS/perlin

.PHONY : CMakeFiles/perlin.dir/build

CMakeFiles/perlin.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/perlin.dir/cmake_clean.cmake
.PHONY : CMakeFiles/perlin.dir/clean

CMakeFiles/perlin.dir/depend:
	cd "/Users/giy/Documents/Max 8/Packages/max-sdk/source/my/perlin" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/giy/Documents/Max 8/Packages/max-sdk/source/my/perlin" "/Users/giy/Documents/Max 8/Packages/max-sdk/source/my/perlin" "/Users/giy/Documents/Max 8/Packages/max-sdk/source/my/perlin" "/Users/giy/Documents/Max 8/Packages/max-sdk/source/my/perlin" "/Users/giy/Documents/Max 8/Packages/max-sdk/source/my/perlin/CMakeFiles/perlin.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/perlin.dir/depend
