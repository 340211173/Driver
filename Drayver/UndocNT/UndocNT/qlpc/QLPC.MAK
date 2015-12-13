# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=qlpc - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to qlpc - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "qlpc - Win32 Release" && "$(CFG)" != "qlpc - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "QLPC.MAK" CFG="qlpc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "qlpc - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "qlpc - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "qlpc - Win32 Debug"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "qlpc - Win32 Release"

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
OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\bin\qlpc.exe"

CLEAN : 
	-@erase "$(INTDIR)\qlpc.obj"
	-@erase "..\bin\qlpc.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\include" /D "WIN32" /D "NDEBUG" /D\
 "_CONSOLE" /Fp"$(INTDIR)/QLPC.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/QLPC.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 ntdll.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /machine:I386 /out:"..\bin\qlpc.exe"
LINK32_FLAGS=ntdll.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/qlpc.pdb" /machine:I386\
 /out:"..\bin\qlpc.exe" 
LINK32_OBJS= \
	"$(INTDIR)\qlpc.obj"

"..\bin\qlpc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "qlpc - Win32 Debug"

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
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\bin\qlpc.exe"

CLEAN : 
	-@erase "$(INTDIR)\qlpc.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\qlpc.pdb"
	-@erase "..\bin\qlpc.exe"
	-@erase "..\bin\qlpc.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "..\include" /D "WIN32" /D\
 "_DEBUG" /D "_CONSOLE" /Fp"$(INTDIR)/QLPC.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/QLPC.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 ntdll.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\bin\qlpc.exe"
LINK32_FLAGS=ntdll.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /subsystem:console /incremental:yes /pdb:"$(OUTDIR)/qlpc.pdb" /debug\
 /machine:I386 /out:"..\bin\qlpc.exe" 
LINK32_OBJS= \
	"$(INTDIR)\qlpc.obj"

"..\bin\qlpc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "qlpc - Win32 Release"
# Name "qlpc - Win32 Debug"

!IF  "$(CFG)" == "qlpc - Win32 Release"

!ELSEIF  "$(CFG)" == "qlpc - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\qlpc.c

!IF  "$(CFG)" == "qlpc - Win32 Release"

DEP_CPP_QLPC_=\
	"..\..\ddk40\inc\alpharef.h"\
	"..\include\undocnt.h"\
	{$(INCLUDE)}"\BUGCODES.H"\
	{$(INCLUDE)}"\EXLEVELS.H"\
	{$(INCLUDE)}"\NTDDK.H"\
	{$(INCLUDE)}"\NTDEF.H"\
	{$(INCLUDE)}"\NTIOLOGC.H"\
	{$(INCLUDE)}"\NTPOAPI.H"\
	{$(INCLUDE)}"\NTSTATUS.H"\
	

"$(INTDIR)\qlpc.obj" : $(SOURCE) $(DEP_CPP_QLPC_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "qlpc - Win32 Debug"

DEP_CPP_QLPC_=\
	"..\..\ddk40\inc\alpharef.h"\
	"..\include\undocnt.h"\
	{$(INCLUDE)}"\BUGCODES.H"\
	{$(INCLUDE)}"\EXLEVELS.H"\
	{$(INCLUDE)}"\NTDDK.H"\
	{$(INCLUDE)}"\NTDEF.H"\
	{$(INCLUDE)}"\NTIOLOGC.H"\
	{$(INCLUDE)}"\NTPOAPI.H"\
	{$(INCLUDE)}"\NTSTATUS.H"\
	

"$(INTDIR)\qlpc.obj" : $(SOURCE) $(DEP_CPP_QLPC_) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
