##############################################################################
# VSTLuaSwig Makefile.
# Copyright (C) 2010      this makefile Hermann Seib.
##############################################################################

# VSTLua requires swig 1.3.33 or higher to wrap the headers.
# This build step could have been wrapped into the VSTLua project, but it's
# cleaner and easier to edit in a separate makefile project (and this also
# allows a really usable error message if swig isn't found ;-)

# Adapt this path if swig is installed elsewhere!
SWIGPATH = ..\..\swigwin-2.0.0

# Adapt this path if the VST SDK is installed elsewhere!
VSTSDK_BASE = ..\..\vstsdk2.4.2


##############################################################################
# Don't touch the rest of this makefile.
##############################################################################

BASEDIR = ..\..\VstLua
SRCDIR  = $(BASEDIR)\src

!IF !EXIST($(SWIGPATH)\swig.exe)
!MESSAGE ERROR: Invalid swig path specified.
!MESSAGE You need swig 1.3.33 or higher to build this project.
!MESSAGE Please visit http://www.swig.org to get the latest Windows version.
!MESSAGE The original uses swigwin-1.3.35.
!MESSAGE Please specify the correct SWIGPATH in VSTLuaSwig.mak!
!ERROR swig missing
!ENDIF

SWIGFLAGS = -Fmicrosoft 
SWIG = $(SWIGPATH)\swig.exe $(SWIGFLAGS)

##############################################################################
# the main target, if nothing else specified
##############################################################################

$(SRCDIR)\swig_gui.cpp:  $(SRCDIR)\swig_gui.i
        $(SWIG) -I$(VSTSDK_BASE) -c++  -lua -o $(SRCDIR)\swig_gui.cpp $(SRCDIR)\swig_gui.i
        
##############################################################################
# the cleaning target
##############################################################################

clean:
        @if exist $(SRCDIR)\swig_gui.cpp del $(SRCDIR)\swig_gui.cpp
