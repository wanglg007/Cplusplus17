# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.12

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2018.2.6\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2018.2.6\bin\cmake\win\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\Development_Github\My_Cplusplus17

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\Development_Github\My_Cplusplus17\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Ex5_05.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Ex5_05.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Ex5_05.dir/flags.make

CMakeFiles/Ex5_05.dir/00_basement/example05/Ex5_05.cpp.obj: CMakeFiles/Ex5_05.dir/flags.make
CMakeFiles/Ex5_05.dir/00_basement/example05/Ex5_05.cpp.obj: ../00_basement/example05/Ex5_05.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\Development_Github\My_Cplusplus17\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Ex5_05.dir/00_basement/example05/Ex5_05.cpp.obj"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\G__~1.EXE  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\Ex5_05.dir\00_basement\example05\Ex5_05.cpp.obj -c D:\Development_Github\My_Cplusplus17\00_basement\example05\Ex5_05.cpp

CMakeFiles/Ex5_05.dir/00_basement/example05/Ex5_05.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Ex5_05.dir/00_basement/example05/Ex5_05.cpp.i"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\Development_Github\My_Cplusplus17\00_basement\example05\Ex5_05.cpp > CMakeFiles\Ex5_05.dir\00_basement\example05\Ex5_05.cpp.i

CMakeFiles/Ex5_05.dir/00_basement/example05/Ex5_05.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Ex5_05.dir/00_basement/example05/Ex5_05.cpp.s"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\Development_Github\My_Cplusplus17\00_basement\example05\Ex5_05.cpp -o CMakeFiles\Ex5_05.dir\00_basement\example05\Ex5_05.cpp.s

# Object files for target Ex5_05
Ex5_05_OBJECTS = \
"CMakeFiles/Ex5_05.dir/00_basement/example05/Ex5_05.cpp.obj"

# External object files for target Ex5_05
Ex5_05_EXTERNAL_OBJECTS =

Ex5_05.exe: CMakeFiles/Ex5_05.dir/00_basement/example05/Ex5_05.cpp.obj
Ex5_05.exe: CMakeFiles/Ex5_05.dir/build.make
Ex5_05.exe: CMakeFiles/Ex5_05.dir/linklibs.rsp
Ex5_05.exe: CMakeFiles/Ex5_05.dir/objects1.rsp
Ex5_05.exe: CMakeFiles/Ex5_05.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\Development_Github\My_Cplusplus17\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Ex5_05.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\Ex5_05.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Ex5_05.dir/build: Ex5_05.exe

.PHONY : CMakeFiles/Ex5_05.dir/build

CMakeFiles/Ex5_05.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\Ex5_05.dir\cmake_clean.cmake
.PHONY : CMakeFiles/Ex5_05.dir/clean

CMakeFiles/Ex5_05.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\Development_Github\My_Cplusplus17 D:\Development_Github\My_Cplusplus17 D:\Development_Github\My_Cplusplus17\cmake-build-debug D:\Development_Github\My_Cplusplus17\cmake-build-debug D:\Development_Github\My_Cplusplus17\cmake-build-debug\CMakeFiles\Ex5_05.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Ex5_05.dir/depend

