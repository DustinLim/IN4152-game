__HOW TO 'INSTALL' GLEW AND SOIL__
in order to load .PNG files into our game

Information found via:
* Set up for GLEW: http://stackoverflow.com/questions/22539047/how-can-i-set-up-freeglut-and-glew-for-use-in-visual-studio-2013
* Set up for SOIL: http://stackoverflow.com/questions/13009049/soil-set-up-in-visual-studio-2010

ZIP-files for download can be find in this folder. You also need `glext.h`

First make sure that GLEW works. You can do this via the following steps:
* Put `glext.h` in C:\Program Files\MVS 12.0\VC\include\GL   - or similar
* Put glew.dll (or it is called glew32.dll) from the zip in C:\Windows\System32
* Put glew32.lib and glew32s.lib (from \lib) in C:\Program Files\MVS 12.0\VC\lib    - or similar
* Put glew.h, glxew.h and wglew.h (from \include\GL) in C:\Program Files\MVS 12.0\VC\include\GL

GLEW than can be used via `#include <GL/glext.h>`


In order to make SOIL work, I've put the unzipped SOIL folder in `C:\` for convenience.
No matter where the folder is (but don't move it afterwards), do the following steps:

* In your visual studio project include path (project -> properties -> vc++ directories -> include directories) add the path to "src" folder of Soil. Same place of -> library directories add the "lib" folder. project -> properties -> Linker -> Input -> Additional Dependencies -> Edit to add "SOIL.lib"
* In my case the lib folder was `C:\SOIL\lib`. Inside this folder there is a '.a' file. You can delete this one.
* Go inside "projects" folder, select a VC* (eg VC8 for VS2012) -> open the visual studio file -> it will open using your visual studio -> click ok -> click ok. Your solution will be Ready.
Now press F5/Run to build and Run. Be careful while doing it, in case your project is using a x64 Debug version then in here select the same before you build the solution.
* There will be folders/files created in the VC* folder. Go inside Debug/x64 (dependent on your project), copy the SOIL.lib file to original "lib" folder (you pointed to in Visual Studio Properties in step 1 <- in my case this became).

More information on SOIL is there via: http://www.lonesock.net/soil.html

You can use SOIL by putting the SOIL.h file in your project and `#include "SOIL.h"`

Hope this works! :)

