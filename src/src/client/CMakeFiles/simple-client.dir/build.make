# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/yjj/test/TrafficGenerator

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/yjj/test/TrafficGenerator/src

# Include any dependencies generated for this target.
include src/client/CMakeFiles/simple-client.dir/depend.make

# Include the progress variables for this target.
include src/client/CMakeFiles/simple-client.dir/progress.make

# Include the compile flags for this target's objects.
include src/client/CMakeFiles/simple-client.dir/flags.make

src/client/CMakeFiles/simple-client.dir/simple-client.c.o: src/client/CMakeFiles/simple-client.dir/flags.make
src/client/CMakeFiles/simple-client.dir/simple-client.c.o: client/simple-client.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yjj/test/TrafficGenerator/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object src/client/CMakeFiles/simple-client.dir/simple-client.c.o"
	cd /home/yjj/test/TrafficGenerator/src/src/client && gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/simple-client.dir/simple-client.c.o   -c /home/yjj/test/TrafficGenerator/src/client/simple-client.c

src/client/CMakeFiles/simple-client.dir/simple-client.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/simple-client.dir/simple-client.c.i"
	cd /home/yjj/test/TrafficGenerator/src/src/client && gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/yjj/test/TrafficGenerator/src/client/simple-client.c > CMakeFiles/simple-client.dir/simple-client.c.i

src/client/CMakeFiles/simple-client.dir/simple-client.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/simple-client.dir/simple-client.c.s"
	cd /home/yjj/test/TrafficGenerator/src/src/client && gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/yjj/test/TrafficGenerator/src/client/simple-client.c -o CMakeFiles/simple-client.dir/simple-client.c.s

src/client/CMakeFiles/simple-client.dir/simple-client.c.o.requires:

.PHONY : src/client/CMakeFiles/simple-client.dir/simple-client.c.o.requires

src/client/CMakeFiles/simple-client.dir/simple-client.c.o.provides: src/client/CMakeFiles/simple-client.dir/simple-client.c.o.requires
	$(MAKE) -f src/client/CMakeFiles/simple-client.dir/build.make src/client/CMakeFiles/simple-client.dir/simple-client.c.o.provides.build
.PHONY : src/client/CMakeFiles/simple-client.dir/simple-client.c.o.provides

src/client/CMakeFiles/simple-client.dir/simple-client.c.o.provides.build: src/client/CMakeFiles/simple-client.dir/simple-client.c.o


# Object files for target simple-client
simple__client_OBJECTS = \
"CMakeFiles/simple-client.dir/simple-client.c.o"

# External object files for target simple-client
simple__client_EXTERNAL_OBJECTS =

../bin/simple-client: src/client/CMakeFiles/simple-client.dir/simple-client.c.o
../bin/simple-client: src/client/CMakeFiles/simple-client.dir/build.make
../bin/simple-client: ../lib/libcommon.a
../bin/simple-client: src/client/CMakeFiles/simple-client.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/yjj/test/TrafficGenerator/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable ../../../bin/simple-client"
	cd /home/yjj/test/TrafficGenerator/src/src/client && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/simple-client.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/client/CMakeFiles/simple-client.dir/build: ../bin/simple-client

.PHONY : src/client/CMakeFiles/simple-client.dir/build

src/client/CMakeFiles/simple-client.dir/requires: src/client/CMakeFiles/simple-client.dir/simple-client.c.o.requires

.PHONY : src/client/CMakeFiles/simple-client.dir/requires

src/client/CMakeFiles/simple-client.dir/clean:
	cd /home/yjj/test/TrafficGenerator/src/src/client && $(CMAKE_COMMAND) -P CMakeFiles/simple-client.dir/cmake_clean.cmake
.PHONY : src/client/CMakeFiles/simple-client.dir/clean

src/client/CMakeFiles/simple-client.dir/depend:
	cd /home/yjj/test/TrafficGenerator/src && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/yjj/test/TrafficGenerator /home/yjj/test/TrafficGenerator/src/client /home/yjj/test/TrafficGenerator/src /home/yjj/test/TrafficGenerator/src/src/client /home/yjj/test/TrafficGenerator/src/src/client/CMakeFiles/simple-client.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/client/CMakeFiles/simple-client.dir/depend

