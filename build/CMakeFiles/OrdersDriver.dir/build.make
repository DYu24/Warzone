# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.18

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "C:\Users\derek\Documents\Projects\COMP 345 Project\Warzone"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "C:\Users\derek\Documents\Projects\COMP 345 Project\Warzone\build"

# Include any dependencies generated for this target.
include CMakeFiles/OrdersDriver.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/OrdersDriver.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/OrdersDriver.dir/flags.make

CMakeFiles/OrdersDriver.dir/src/orders/OrdersDriver.cpp.obj: CMakeFiles/OrdersDriver.dir/flags.make
CMakeFiles/OrdersDriver.dir/src/orders/OrdersDriver.cpp.obj: ../src/orders/OrdersDriver.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="C:\Users\derek\Documents\Projects\COMP 345 Project\Warzone\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/OrdersDriver.dir/src/orders/OrdersDriver.cpp.obj"
	C:\PROGRA~1\mingw-w64\x86_64-8.1.0-posix-seh-rt_v6-rev0\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\OrdersDriver.dir\src\orders\OrdersDriver.cpp.obj -c "C:\Users\derek\Documents\Projects\COMP 345 Project\Warzone\src\orders\OrdersDriver.cpp"

CMakeFiles/OrdersDriver.dir/src/orders/OrdersDriver.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/OrdersDriver.dir/src/orders/OrdersDriver.cpp.i"
	C:\PROGRA~1\mingw-w64\x86_64-8.1.0-posix-seh-rt_v6-rev0\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "C:\Users\derek\Documents\Projects\COMP 345 Project\Warzone\src\orders\OrdersDriver.cpp" > CMakeFiles\OrdersDriver.dir\src\orders\OrdersDriver.cpp.i

CMakeFiles/OrdersDriver.dir/src/orders/OrdersDriver.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/OrdersDriver.dir/src/orders/OrdersDriver.cpp.s"
	C:\PROGRA~1\mingw-w64\x86_64-8.1.0-posix-seh-rt_v6-rev0\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "C:\Users\derek\Documents\Projects\COMP 345 Project\Warzone\src\orders\OrdersDriver.cpp" -o CMakeFiles\OrdersDriver.dir\src\orders\OrdersDriver.cpp.s

# Object files for target OrdersDriver
OrdersDriver_OBJECTS = \
"CMakeFiles/OrdersDriver.dir/src/orders/OrdersDriver.cpp.obj"

# External object files for target OrdersDriver
OrdersDriver_EXTERNAL_OBJECTS =

OrdersDriver.exe: CMakeFiles/OrdersDriver.dir/src/orders/OrdersDriver.cpp.obj
OrdersDriver.exe: CMakeFiles/OrdersDriver.dir/build.make
OrdersDriver.exe: libWarzoneLib.a
OrdersDriver.exe: CMakeFiles/OrdersDriver.dir/linklibs.rsp
OrdersDriver.exe: CMakeFiles/OrdersDriver.dir/objects1.rsp
OrdersDriver.exe: CMakeFiles/OrdersDriver.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="C:\Users\derek\Documents\Projects\COMP 345 Project\Warzone\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable OrdersDriver.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\OrdersDriver.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/OrdersDriver.dir/build: OrdersDriver.exe

.PHONY : CMakeFiles/OrdersDriver.dir/build

CMakeFiles/OrdersDriver.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\OrdersDriver.dir\cmake_clean.cmake
.PHONY : CMakeFiles/OrdersDriver.dir/clean

CMakeFiles/OrdersDriver.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" "C:\Users\derek\Documents\Projects\COMP 345 Project\Warzone" "C:\Users\derek\Documents\Projects\COMP 345 Project\Warzone" "C:\Users\derek\Documents\Projects\COMP 345 Project\Warzone\build" "C:\Users\derek\Documents\Projects\COMP 345 Project\Warzone\build" "C:\Users\derek\Documents\Projects\COMP 345 Project\Warzone\build\CMakeFiles\OrdersDriver.dir\DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/OrdersDriver.dir/depend
