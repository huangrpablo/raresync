# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/paulo/Documents/SP/code/repo/raresync

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/paulo/Documents/SP/code/repo/raresync/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/raresync.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/raresync.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/raresync.dir/flags.make

CMakeFiles/raresync.dir/main.cpp.o: CMakeFiles/raresync.dir/flags.make
CMakeFiles/raresync.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/paulo/Documents/SP/code/repo/raresync/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/raresync.dir/main.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/raresync.dir/main.cpp.o -c /Users/paulo/Documents/SP/code/repo/raresync/main.cpp

CMakeFiles/raresync.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/raresync.dir/main.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/paulo/Documents/SP/code/repo/raresync/main.cpp > CMakeFiles/raresync.dir/main.cpp.i

CMakeFiles/raresync.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/raresync.dir/main.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/paulo/Documents/SP/code/repo/raresync/main.cpp -o CMakeFiles/raresync.dir/main.cpp.s

CMakeFiles/raresync.dir/src/core.cpp.o: CMakeFiles/raresync.dir/flags.make
CMakeFiles/raresync.dir/src/core.cpp.o: ../src/core.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/paulo/Documents/SP/code/repo/raresync/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/raresync.dir/src/core.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/raresync.dir/src/core.cpp.o -c /Users/paulo/Documents/SP/code/repo/raresync/src/core.cpp

CMakeFiles/raresync.dir/src/core.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/raresync.dir/src/core.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/paulo/Documents/SP/code/repo/raresync/src/core.cpp > CMakeFiles/raresync.dir/src/core.cpp.i

CMakeFiles/raresync.dir/src/core.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/raresync.dir/src/core.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/paulo/Documents/SP/code/repo/raresync/src/core.cpp -o CMakeFiles/raresync.dir/src/core.cpp.s

CMakeFiles/raresync.dir/src/network.cpp.o: CMakeFiles/raresync.dir/flags.make
CMakeFiles/raresync.dir/src/network.cpp.o: ../src/network.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/paulo/Documents/SP/code/repo/raresync/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/raresync.dir/src/network.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/raresync.dir/src/network.cpp.o -c /Users/paulo/Documents/SP/code/repo/raresync/src/network.cpp

CMakeFiles/raresync.dir/src/network.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/raresync.dir/src/network.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/paulo/Documents/SP/code/repo/raresync/src/network.cpp > CMakeFiles/raresync.dir/src/network.cpp.i

CMakeFiles/raresync.dir/src/network.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/raresync.dir/src/network.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/paulo/Documents/SP/code/repo/raresync/src/network.cpp -o CMakeFiles/raresync.dir/src/network.cpp.s

CMakeFiles/raresync.dir/src/network_peer.cpp.o: CMakeFiles/raresync.dir/flags.make
CMakeFiles/raresync.dir/src/network_peer.cpp.o: ../src/network_peer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/paulo/Documents/SP/code/repo/raresync/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/raresync.dir/src/network_peer.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/raresync.dir/src/network_peer.cpp.o -c /Users/paulo/Documents/SP/code/repo/raresync/src/network_peer.cpp

CMakeFiles/raresync.dir/src/network_peer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/raresync.dir/src/network_peer.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/paulo/Documents/SP/code/repo/raresync/src/network_peer.cpp > CMakeFiles/raresync.dir/src/network_peer.cpp.i

CMakeFiles/raresync.dir/src/network_peer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/raresync.dir/src/network_peer.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/paulo/Documents/SP/code/repo/raresync/src/network_peer.cpp -o CMakeFiles/raresync.dir/src/network_peer.cpp.s

CMakeFiles/raresync.dir/src/network_server.cpp.o: CMakeFiles/raresync.dir/flags.make
CMakeFiles/raresync.dir/src/network_server.cpp.o: ../src/network_server.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/paulo/Documents/SP/code/repo/raresync/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/raresync.dir/src/network_server.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/raresync.dir/src/network_server.cpp.o -c /Users/paulo/Documents/SP/code/repo/raresync/src/network_server.cpp

CMakeFiles/raresync.dir/src/network_server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/raresync.dir/src/network_server.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/paulo/Documents/SP/code/repo/raresync/src/network_server.cpp > CMakeFiles/raresync.dir/src/network_server.cpp.i

CMakeFiles/raresync.dir/src/network_server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/raresync.dir/src/network_server.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/paulo/Documents/SP/code/repo/raresync/src/network_server.cpp -o CMakeFiles/raresync.dir/src/network_server.cpp.s

CMakeFiles/raresync.dir/example/run_instance.cpp.o: CMakeFiles/raresync.dir/flags.make
CMakeFiles/raresync.dir/example/run_instance.cpp.o: ../example/run_instance.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/paulo/Documents/SP/code/repo/raresync/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/raresync.dir/example/run_instance.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/raresync.dir/example/run_instance.cpp.o -c /Users/paulo/Documents/SP/code/repo/raresync/example/run_instance.cpp

CMakeFiles/raresync.dir/example/run_instance.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/raresync.dir/example/run_instance.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/paulo/Documents/SP/code/repo/raresync/example/run_instance.cpp > CMakeFiles/raresync.dir/example/run_instance.cpp.i

CMakeFiles/raresync.dir/example/run_instance.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/raresync.dir/example/run_instance.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/paulo/Documents/SP/code/repo/raresync/example/run_instance.cpp -o CMakeFiles/raresync.dir/example/run_instance.cpp.s

CMakeFiles/raresync.dir/example/gen_conf.cpp.o: CMakeFiles/raresync.dir/flags.make
CMakeFiles/raresync.dir/example/gen_conf.cpp.o: ../example/gen_conf.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/paulo/Documents/SP/code/repo/raresync/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/raresync.dir/example/gen_conf.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/raresync.dir/example/gen_conf.cpp.o -c /Users/paulo/Documents/SP/code/repo/raresync/example/gen_conf.cpp

CMakeFiles/raresync.dir/example/gen_conf.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/raresync.dir/example/gen_conf.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/paulo/Documents/SP/code/repo/raresync/example/gen_conf.cpp > CMakeFiles/raresync.dir/example/gen_conf.cpp.i

CMakeFiles/raresync.dir/example/gen_conf.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/raresync.dir/example/gen_conf.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/paulo/Documents/SP/code/repo/raresync/example/gen_conf.cpp -o CMakeFiles/raresync.dir/example/gen_conf.cpp.s

# Object files for target raresync
raresync_OBJECTS = \
"CMakeFiles/raresync.dir/main.cpp.o" \
"CMakeFiles/raresync.dir/src/core.cpp.o" \
"CMakeFiles/raresync.dir/src/network.cpp.o" \
"CMakeFiles/raresync.dir/src/network_peer.cpp.o" \
"CMakeFiles/raresync.dir/src/network_server.cpp.o" \
"CMakeFiles/raresync.dir/example/run_instance.cpp.o" \
"CMakeFiles/raresync.dir/example/gen_conf.cpp.o"

# External object files for target raresync
raresync_EXTERNAL_OBJECTS =

raresync: CMakeFiles/raresync.dir/main.cpp.o
raresync: CMakeFiles/raresync.dir/src/core.cpp.o
raresync: CMakeFiles/raresync.dir/src/network.cpp.o
raresync: CMakeFiles/raresync.dir/src/network_peer.cpp.o
raresync: CMakeFiles/raresync.dir/src/network_server.cpp.o
raresync: CMakeFiles/raresync.dir/example/run_instance.cpp.o
raresync: CMakeFiles/raresync.dir/example/gen_conf.cpp.o
raresync: CMakeFiles/raresync.dir/build.make
raresync: ../lib/libbls384_256.a
raresync: CMakeFiles/raresync.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/paulo/Documents/SP/code/repo/raresync/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Linking CXX executable raresync"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/raresync.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/raresync.dir/build: raresync
.PHONY : CMakeFiles/raresync.dir/build

CMakeFiles/raresync.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/raresync.dir/cmake_clean.cmake
.PHONY : CMakeFiles/raresync.dir/clean

CMakeFiles/raresync.dir/depend:
	cd /Users/paulo/Documents/SP/code/repo/raresync/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/paulo/Documents/SP/code/repo/raresync /Users/paulo/Documents/SP/code/repo/raresync /Users/paulo/Documents/SP/code/repo/raresync/cmake-build-debug /Users/paulo/Documents/SP/code/repo/raresync/cmake-build-debug /Users/paulo/Documents/SP/code/repo/raresync/cmake-build-debug/CMakeFiles/raresync.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/raresync.dir/depend

