# Microsoft Developer Studio Project File - Name="dlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dlib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dlib.mak" CFG="dlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "dlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../utils" /I "../utils/win32" /I "../intrp" /I "../utils/win32/jvm" /I "../uzip" /I "../cmn" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__JDEPENDS__" /D "__EXPAND_CODE__" /D "__JVM_CHECKTOOL_TEST_ENV__" /D "__I386__" /D "__THREAD__" /D "__CP_TRACE_ON__" /D "DLL" /D "__NO_ACC_FLAGS__" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "dlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "../intrp" /I "../cmn" /I "../utils" /I "../utils/win32" /I "../utils/win32/jvm" /I "../uzip" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__JDEPENDS__" /D "__DEBUG__" /D "__EXPAND_CODE__" /D "__JVM_CHECKTOOL_TEST_ENV__" /D "__I386__" /D "__THREAD__" /D "__CP_TRACE_ON__" /D "DLL" /D "__NO_ACC_FLAGS__" /D "__WINSOCK__" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "dlib - Win32 Release"
# Name "dlib - Win32 Debug"
# Begin Source File

SOURCE=..\utils\bintree.c
# End Source File
# Begin Source File

SOURCE=..\utils\cmdproc.c
# End Source File
# Begin Source File

SOURCE=..\utils\hashtabl.c
# End Source File
# Begin Source File

SOURCE=..\intrp\jvmconfg.c
# End Source File
# Begin Source File

SOURCE=..\intrp\Jvmerr.c
# End Source File
# Begin Source File

SOURCE=..\intrp\Jvmexcpt.c
# End Source File
# Begin Source File

SOURCE=..\intrp\jvmloadr.c
# End Source File
# Begin Source File

SOURCE=..\intrp\Jvmmem.c

!IF  "$(CFG)" == "dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "dlib - Win32 Debug"

# ADD CPP /D "__USE_CRT_MEM__"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\intrp\Jvmparse.c
# End Source File
# Begin Source File

SOURCE=..\intrp\Jvmrt.c
# End Source File
# Begin Source File

SOURCE=..\intrp\Jvmsprng.c
# End Source File
# Begin Source File

SOURCE=..\intrp\Jvmsys.c
# End Source File
# Begin Source File

SOURCE=..\intrp\Jvmutil.c
# End Source File
# Begin Source File

SOURCE=..\utils\linklist.c
# End Source File
# Begin Source File

SOURCE=..\utils\mem.c
# End Source File
# Begin Source File

SOURCE=..\utils\win32\osapi.c
# End Source File
# Begin Source File

SOURCE=..\utils\pool.c
# End Source File
# Begin Source File

SOURCE=..\utils\stream.c
# End Source File
# Begin Source File

SOURCE=..\utils\win32\util.c
# End Source File
# End Target
# End Project
