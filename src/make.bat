


rem clean up
del *.o


gcc lbitlib.c -c -Ilua-5.1.4/src

windres vstlua.rc -o resource.o

set VSTGUI_PATH=c:\devel\vstsdk2.4.2\vstgui.sf\vstgui
set VSTSDK_BASE=c:\devel\vstsdk2.4.2
set VSTSDK_PATH=%VSTSDK_BASE%\public.sdk\source\vst2.x
set OSC_PATH=oscpack\osc


rem set SDL_LIB=d:\misc\msys\1.0\local\lib
rem set SDLCONSOLE_LIB=d:\misc\msys\1.0\local\lib
rem set SDL_INC=d:\misc\msys\1.0\local\include\SDL
rem set SDLCONSOLE_INC=d:\misc\msys\1.0\local\include\SDL_console


set LIBS=-lole32 -lrpcrt4 -loleaut32  -luuid -lwinspool -lole32 -lshell32  -llua -lluasocket  -lwsock32 -lws2_32   -lwinmm 


set LIBDIRS=-L. -Llua-5.1.4/src -Lluasocket-2.0.2/src -L%SDL_LIB% -L%SDLCONSOLE_LIB%
set INCDIRS=-I. -Iaddons/lookup -Ioscpack -I%VSTSDK_BASE% -Iluasocket-2.0.2/src -Ilua-5.1.4/src -I%VSTSDK_PATH% 

set CFLAGS=-g3  -DWINDOWS -mwindows 
rem set CFLAGS=-O4 -s  -DWINDOWS  -mwindows 


rem You need to use swig 1.3.33 or higher to wrap the headers; 
set SWIG_PATH=c:\devel\swigwin-2.0.0
call swigwrap.bat

gcc -c %INCDIRS% %CFLAGS% addons/random/random.c addons/random/lrandom.c   -Ilua-5.1.4/src
gcc -c %INCDIRS% %CFLAGS% addons/struct/struct.c -Ilua-5.1.4/src
gcc -c %INCDIRS% %CFLAGS% lookup3.c -I. -Ilua-5.1.4/src
g++ -Wall -c %INCDIRS%  %CFLAGS% luahash.cpp -I. -Ilua-5.1.4/src
g++ -Wall -c %INCDIRS%  %CFLAGS% console.cpp
g++ -DSTATIC_LINKED -c %INCDIRS%   %CFLAGS% -Wall customcontrol.cpp sysfuncs.cpp vstlua.cpp luautils.cpp luaeditor.cpp xypad.cpp luacalls.cpp luagui.cpp linemanager.cpp osclua.cpp swig_gui.cpp sharedmem.cpp nativemidi.cpp


g++ -c %INCDIRS%  %CFLAGS%  oscpack/osc/OscTypes.cpp oscpack/osc/OscReceivedElements.cpp oscpack/osc/OscOutboundPacketStream.cpp 


g++ -c %INCDIRS% %CFLAGS% %VSTSDK_PATH%\audioeffectx.cpp %VSTSDK_PATH%\audioeffect.cpp  %VSTGUI_PATH%\aeffguieditor.cpp %VSTGUI_PATH%\vstcontrols.cpp %VSTGUI_PATH%\vstgui.cpp %VSTGUI_PATH%\ctabview.cpp  %VSTGUI_PATH%\cfileselector.cpp  %VSTSDK_PATH%\vstplugmain.cpp

g++ -static -shared %CFLAGS% sharedmem.o console.o struct.o lrandom.o random.o resource.o lbitlib.o lookup3.o luahash.o customcontrol.o sysfuncs.o vstlua.o luautils.o luaeditor.o xypad.o luacalls.o luagui.o nativemidi.o linemanager.o osclua.o osctypes.o oscreceivedelements.o oscoutboundpacketstream.o audioeffectx.o audioeffect.o aeffguieditor.o vstcontrols.o vstgui.o ctabview.o swig_gui.o cfileselector.o vstplugmain.o -o vstlua.dll %LIBDIRS%  vstplug.def %LIBS%  



