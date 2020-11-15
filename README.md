# onion
A thing using C++ and OpenGL.

## Build Instructions (Visual Studio)
First, you will need to download the following libraries:
* <a href="http://glew.sourceforge.net/">GLEW</a> (Minimum version: 2.1.0)
* <a href="https://www.glfw.org/">GLFW</a> (Minimum version: 3.2.1)
* <a href="https://github.com/kbranigan/Simple-OpenGL-Image-Library">SOIL</a>

The following instructions were written for VS2015, and may be different for other versions of Visual Studio.

### All Projects
For all projects, under the General tab:
* Output Directory = $(SolutionDir)bin\\$(Configuration)\

### onions (Static Library)
Under VC++ Directories, add the following to Include Directories:
* C:\\[insert path here]\glew-2.1.0-win32\glew-2.1.0\include
* C:\\[insert path here]\soil\Simple OpenGL Image Library\src
* C:\\[insert path here]\glfw-3.2.1.bin\glfw-3.2.1.bin.WIN32\include

Under VC++ Directories, add the following to Library Directories:
* C:\\[insert path here]\glew-2.1.0-win32\glew-2.1.0\lib\Release\Win32
* C:\\[insert path here]\soil\Simple OpenGL Image Library\lib
* C:\\[insert path here]\glfw-3.2.1.bin\glfw-3.2.1.bin.WIN32\lib-vc[year corresponding to your version of Visual Studio]

Under C/C++, add the following to Preprocessor Definitions:
* WIN32
* GLEW_STATIC

Under Librarian, add the following to Additional Dependencies:
* opengl32.lib
* glew32s.lib
* glfw3.lib
* SOIL.lib

### onion (Console Executable)
Under Debugging, set the Working Directory to $(SolutionDir)

Under VC++ Directories, add the following to Include Directories:
* $(SolutionDir)onions\include\

Under VC++ Directories, add the following to Library Directories:
* $(SolutionDir)bin\\$(Configuration)

Under Linker, add the following to Additional Dependencies:
* onions.lib
