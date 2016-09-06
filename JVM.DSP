# Microsoft Developer Studio Project File - Name="jvm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=jvm - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "jvm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "jvm.mak" CFG="jvm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "jvm - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "jvm - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "jvm - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "__I386__" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "jvm - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "__DEBUG__" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "__I386__" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# Begin Custom Build
TargetName=jvm
InputPath=.\Debug\jvm.exe
SOURCE=$(InputPath)

"out.out" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	.\checktool\debug\checktool.exe $(TargetName) CHECK,CHECK

# End Custom Build

!ENDIF 

# Begin Target

# Name "jvm - Win32 Release"
# Name "jvm - Win32 Debug"
# Begin Source File

SOURCE=.\bintree.c
# End Source File
# Begin Source File

SOURCE=.\cmnmcros.h
# End Source File
# Begin Source File

SOURCE=.\Hashtabl.c
# End Source File
# Begin Source File

SOURCE=..\Jasyscls.txt
# End Source File
# Begin Source File

SOURCE=.\Jvmclass.c
# End Source File
# Begin Source File

SOURCE=.\jvmconfg.c
# End Source File
# Begin Source File

SOURCE=.\jvmerr.c
# End Source File
# Begin Source File

SOURCE=.\jvmexcpt.c
# End Source File
# Begin Source File

SOURCE=.\jvmloadr.c
# End Source File
# Begin Source File

SOURCE=.\Jvmmain.c
# End Source File
# Begin Source File

SOURCE=.\jvmmem.c
# End Source File
# Begin Source File

SOURCE=.\jvmparse.c
# End Source File
# Begin Source File

SOURCE=.\jvmrt.c
# End Source File
# Begin Source File

SOURCE=.\Jvmsprng.c
# End Source File
# Begin Source File

SOURCE=.\Jvmsys.c
# End Source File
# Begin Source File

SOURCE=.\jvmutil.c
# End Source File
# Begin Source File

SOURCE=.\Jvmw32os.c
# End Source File
# Begin Source File

SOURCE=.\linklist.c
# End Source File
# Begin Source File

SOURCE=.\snippets.txt
# End Source File
# Begin Source File

SOURCE=.\Snprissu.txt
# End Source File
# Begin Source File

SOURCE=.\snprtdad.txt
# End Source File
# Begin Source File

SOURCE=.\snprtodo.txt
# End Source File
# End Target
# End Project
