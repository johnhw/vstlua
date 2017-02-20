## VS2005 Port of VSTLua: Setup Instructions
* Sean Dunn, seanedunn@yahoo.com
* Last updated: 2007/11/18
* Adjusted to 0.06-beta2 sources
* Hermann Seib, office@hermannseib.com
* Last updated: 2010/08/11

To build VSTLua with Visual Studio 2005, place your folders as shown in the diagram below.

An additional tool - swig - is needed for generation, in at least V1.3.33.
You can find the latest version at http://www.swig.org - on the main page,
they usually got a link to a .zip file including a pre-built Windows executable.

The VSTLuaSwig project expects the executable according to the diagram outlined below;
if this doesn't match your installation, you have to adapt the path in the makefile included
in the project.

You must obtain the vstsdk2.4.2 folder separately.

## VSTSDK 2.4.2
Go to http://www.steinberg.net/324+M52087573ab0.html (if this link doesn't work, go to www.steinberg.net, then Support, and look for the 3rd party box on the lower right). Then download the VST Audio Plug-Ins SDK.

## Note on porting this to VS2005

Search for 'VS2005' to find code that has been changed from the original to suit VS2005 and Win32's needs.



    +---VSTLua.05
    |   +---Lua-5.1.4
    |   +---luasocket-2.0.2
    |   +---oscpack
    |   \---VSTLua
    +---VSTLua
    |   \---src
    |       +---images
    |       |   \---small
    |       +---lua
    |       |   \---lib
    |       |       +---sockets
    |       |       \---stdlib
    |       |           +---modules
    |       |           \---utils
    |       +---lua-5.1.4
    |       |   +---doc
    |       |   +---etc
    |       |   +---src
    |       |   \---test
    |       +---luasocket-2.0.2
    |       |   +---doc
    |       |   +---etc
    |       |   +---samples
    |       |   +---src
    |       |   +---test
    |       |   \---_UpgradeReport_Files
    |       +---oscpack
    |       |   +---bin
    |       |   +---Debug
    |       |   +---examples
    |       |   +---ip
    |       |   |   +---posix
    |       |   |   \---win32
    |       |   +---osc
    |       |   +---Release
    |       |   \---tests
    |       +---ps
    |       +---scripts
    |       |   +---contrib
    |       |   \---netosc
    |       \---web
    +---vstsdk2.4.2 ***
    |   +---artwork
    |   +---bin
    |   |   +---mac
    |   |   |   +---VSTMonitor.vst
    |   |   |   |   \---Contents
    |   |   |   |       +---MacOS
    |   |   |   |       \---Resources
    |   |   |   \---VSTParamTool.app
    |   |   |       \---Contents
    |   |   |           +---MacOS
    |   |   |           \---Resources
    |   |   \---win
    |   +---Debug
    |   +---doc
    |   |   +---gfx
    |   |   \---html
    |   +---pluginterfaces
    |   |   \---vst2.x
    |   +---public.sdk
    |   |   +---samples
    |   |   |   \---vst2.x
    |   |   |       +---adelay
    |   |   |       |   +---editor
    |   |   |       |   |   \---resources
    |   |   |       |   \---win
    |   |   |       +---again
    |   |   |       |   +---source
    |   |   |       |   \---win
    |   |   |       +---mac
    |   |   |       |   \---vst 2.4 examples.xcodeproj
    |   |   |       +---minihost
    |   |   |       |   +---source
    |   |   |       |   \---win
    |   |   |       +---vstxsynth
    |   |   |       |   +---resource
    |   |   |       |   +---source
    |   |   |       |   \---win
    |   |   |       |       \---Debug
    |   |   |       +---win
    |   |   |       |   \---debug
    |   |   |       +---win.vc2003
    |   |   |       \---win.vc6
    |   |   \---source
    |   |       \---vst2.x
    |   \---vstgui.sf
    |       +---drawtest
    |       |   +---mac
    |       |   |   +---drawtest.xcode
    |       |   |   \---drawtest.xcodeproj
    |       |   +---resources
    |       |   +---source
    |       |   +---win
    |       |   \---win.vc6
    |       \---vstgui
    |           \---Documentation
    |               \---html
    \---swigwin-2.0.0 ***
        +...

## Changes

2010/08/11 Hermann Seib:

    Made the VS2005 project, which obviously was for an earlier version, compilable again.
    Added the VSTLuaSwig project for creation of the necessary "glue" between VSTGui and Lua.
    Using VST SDK 2.4.2 instead of VST SDK 2.4
    Using Lua 5.1.4 instead of Lua 5.1.2, including the same set of patches (patch files are
    included in the VSTLua\src\lua-5.1.4 folder).
    Using swigwin-2.0.0 instead of 1.3.35.
    And finally, some source code changes to speed up things and cure some crash situations.

2007/11/18

    Added .pdb creation for DLL so you can debug in VS2005
    Rearranged .lib dependencies in a more standard way
    Now using VSTGUI that comes with VST SDK.

2007/11/13

    Initial VS2005 port
