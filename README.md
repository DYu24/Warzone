# Warzone

## Setup Instructions (Windows)
Make sure to have [CMake](https://cmake.org/download/) and [Mingw-w64](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/installer/mingw-w64-install.exe/download) installed before following the steps below in the command line.

1. Clone this repo on your local machine.
2. Create a `build/` directory at the root of the project.
3. Navigate into the `build/` directory. e.g. `cd build/`
4. Configure the project using `cmake ../ -G "MinGW Makefiles"`
5. Build the project using `cmake --build .`
6. Run any of the generated executables inside the `build/` directory.