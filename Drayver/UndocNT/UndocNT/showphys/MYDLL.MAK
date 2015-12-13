# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=mydll - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to mydll - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mydll - Win32 Release" && "$(CFG)" != "mydll - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "MYDLL.MAK" CFG="mydll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mydll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mydll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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
# PROP Target_Last_Scanned "mydll - Win32 Debug"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mydll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mydll___"
# PROP BASE Intermediate_Dir "mydll___"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "release"
# PROP Intermediate_Dir "release"
# PROP Target_Dir ""
OUTDIR=.\release
INTDIR=.\release

ALL : "..\bin\mydll.dll"

CLEAN : 
	-@erase "$(INTDIR)\mydll.obj"
	-@erase "$(OUTDIR)\MYDLL.exp"
	-@erase "$(OUTDIR)\MYDLL.lib"
	-@erase "..\bin\mydll.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/MYDLL.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/MYDLL.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:0x20000000 /subsystem:windows /dll /machine:I386 /out:"..\bin\mydll.dll" /implib:"..\lib\mydll.lib" /section:MyCodeSeg1,RE /section:MyCodeSeg2,RE /section:SharedDataSeg,RWS /section:NonSharedDataSeg,RW
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /base:0x20000000 /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/mydll.pdb" /machine:I386 /out:"..\bin\mydll.dll"\
 /implib:"..\lib\mydll.lib" /section:MyCodeSeg1,RE /section:MyCodeSeg2,RE\
 /section:SharedDataSeg,RWS /section:NonSharedDataSeg,RW 
LINK32_OBJS= \
	"$(INTDIR)\mydll.obj"

"..\bin\mydll.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mydll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "mydll__0"
# PROP BASE Intermediate_Dir "mydll__0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "mydll__0"
# PROP Intermediate_Dir "mydll__0"
# PROP Target_Dir ""
OUTDIR=.\mydll__0
INTDIR=.\mydll__0

ALL : "..\bin\mydll.dll"

CLEAN : 
	-@erase "..\bin\mydll.dll"
	-@erase "..\bin\mydll.ilk"
	-@erase "..\lib\mydll.exp"
	-@erase "..\lib\mydll.lib"
	-@erase ".\debug\mydll.obj"
	-@erase ".\debug\mydll.pdb"
	-@erase ".\debug\vc40.idb"
	-@erase ".\debug\vc40.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"debug/MYDLL.pch" /YX /Fo"debug/" /Fd"debug/" /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"debug/MYDLL.pch" /YX /Fo"debug/" /Fd"debug/" /c 
CPP_OBJS=.\debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/MYDLL.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /pdb:"debug/mydll.pdb" /debug /machine:I386 /out:"..\bin\mydll.dll" /implib:"..\lib\mydll.lib"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"debug/mydll.pdb" /debug /machine:I386 /out:"..\bin\mydll.dll"\
 /implib:"..\lib\mydll.lib" 
LINK32_OBJS= \
	".\debug\mydll.obj"

"..\bin\mydll.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "mydll - Win32 Release"
# Name "mydll - Win32 Debug"

!IF  "$(CFG)" == "mydll - Win32 Release"

!ELSEIF  "$(CFG)" == "mydll - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\mydll.c

!IF  "$(CFG)" == "mydll - Win32 Release"


"$(INTDIR)\mydll.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mydll - Win32 Debug"


".\debug\mydll.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
