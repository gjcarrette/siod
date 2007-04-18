# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103
# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=wcsiod - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to wcsiod - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "siod - Win32 Release" && "$(CFG)" != "siod - Win32 Debug" &&\
 "$(CFG)" != "libsiod - Win32 Release" && "$(CFG)" != "libsiod - Win32 Debug" &&\
 "$(CFG)" != "parser_pratt - Win32 Release" && "$(CFG)" !=\
 "parser_pratt - Win32 Debug" && "$(CFG)" != "tar - Win32 Release" && "$(CFG)"\
 != "tar - Win32 Debug" && "$(CFG)" != "ss - Win32 Release" && "$(CFG)" !=\
 "ss - Win32 Debug" && "$(CFG)" != "regex - Win32 Release" && "$(CFG)" !=\
 "regex - Win32 Debug" && "$(CFG)" != "dummy - Win32 Release" && "$(CFG)" !=\
 "dummy - Win32 Debug" && "$(CFG)" != "wcsiod - Win32 Release" && "$(CFG)" !=\
 "wcsiod - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "siod.mak" CFG="wcsiod - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "siod - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "siod - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "libsiod - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libsiod - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "parser_pratt - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "parser_pratt - Win32 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tar - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tar - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ss - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ss - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "regex - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "regex - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dummy - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "dummy - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "wcsiod - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "wcsiod - Win32 Debug" (based on "Win32 (x86) Console Application")
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
# PROP Target_Last_Scanned "libsiod - Win32 Debug"

!IF  "$(CFG)" == "siod - Win32 Release"

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

ALL : "libsiod - Win32 Release" "$(OUTDIR)\siod.exe"

CLEAN : 
	-@erase ".\Release\siod.exe"
	-@erase ".\Release\siod.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/siod.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=

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

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/siod.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/siod.pdb" /machine:I386 /out:"$(OUTDIR)/siod.exe" 
LINK32_OBJS= \
	"$(INTDIR)/siod.obj" \
	".\Release\libsiod.lib"

"$(OUTDIR)\siod.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "siod - Win32 Debug"

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

ALL : "libsiod - Win32 Debug" "$(OUTDIR)\siod.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\siod.exe"
	-@erase ".\Debug\siod.obj"
	-@erase ".\Debug\siod.ilk"
	-@erase ".\Debug\siod.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/siod.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=

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

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/siod.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/siod.pdb" /debug /machine:I386 /out:"$(OUTDIR)/siod.exe" 
LINK32_OBJS= \
	"$(INTDIR)/siod.obj" \
	".\Debug\libsiod.lib"

"$(OUTDIR)\siod.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libsiod - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libsiod\Release"
# PROP BASE Intermediate_Dir "libsiod\Release"
# PROP BASE Target_Dir "libsiod"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir "libsiod"
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\libsiod.dll"

CLEAN : 
	-@erase ".\Release\libsiod.dll"
	-@erase ".\Release\md5.obj"
	-@erase ".\Release\slib.obj"
	-@erase ".\Release\trace.obj"
	-@erase ".\Release\slibu.obj"
	-@erase ".\Release\sliba.obj"
	-@erase ".\Release\libsiod.lib"
	-@erase ".\Release\libsiod.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/libsiod.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/libsiod.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/libsiod.pdb" /machine:I386 /def:".\libsiod.def"\
 /out:"$(OUTDIR)/libsiod.dll" /implib:"$(OUTDIR)/libsiod.lib" 
DEF_FILE= \
	".\libsiod.def"
LINK32_OBJS= \
	"$(INTDIR)/md5.obj" \
	"$(INTDIR)/slib.obj" \
	"$(INTDIR)/trace.obj" \
	"$(INTDIR)/slibu.obj" \
	"$(INTDIR)/sliba.obj"

"$(OUTDIR)\libsiod.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libsiod - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "libsiod\Debug"
# PROP BASE Intermediate_Dir "libsiod\Debug"
# PROP BASE Target_Dir "libsiod"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir "libsiod"
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\libsiod.dll"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\libsiod.dll"
	-@erase ".\Debug\sliba.obj"
	-@erase ".\Debug\trace.obj"
	-@erase ".\Debug\slibu.obj"
	-@erase ".\Debug\slib.obj"
	-@erase ".\Debug\md5.obj"
	-@erase ".\Debug\libsiod.ilk"
	-@erase ".\Debug\libsiod.lib"
	-@erase ".\Debug\libsiod.exp"
	-@erase ".\Debug\libsiod.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/libsiod.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/libsiod.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/libsiod.pdb" /debug /machine:I386 /def:".\libsiod.def"\
 /out:"$(OUTDIR)/libsiod.dll" /implib:"$(OUTDIR)/libsiod.lib" 
DEF_FILE= \
	".\libsiod.def"
LINK32_OBJS= \
	"$(INTDIR)/sliba.obj" \
	"$(INTDIR)/trace.obj" \
	"$(INTDIR)/slibu.obj" \
	"$(INTDIR)/slib.obj" \
	"$(INTDIR)/md5.obj"

"$(OUTDIR)\libsiod.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "parser_pratt - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "parser_pratt\Release"
# PROP BASE Intermediate_Dir "parser_pratt\Release"
# PROP BASE Target_Dir "parser_pratt"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir "parser_pratt"
OUTDIR=.\Release
INTDIR=.\Release

ALL : "libsiod - Win32 Release" "$(OUTDIR)\parser_pratt.dll"

CLEAN : 
	-@erase ".\Release\parser_pratt.dll"
	-@erase ".\Release\parser_pratt.obj"
	-@erase ".\Release\parser_pratt.lib"
	-@erase ".\Release\parser_pratt.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/parser_pratt.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/parser_pratt.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/parser_pratt.pdb" /machine:I386 /def:".\parser_pratt.def"\
 /out:"$(OUTDIR)/parser_pratt.dll" /implib:"$(OUTDIR)/parser_pratt.lib" 
DEF_FILE= \
	".\parser_pratt.def"
LINK32_OBJS= \
	"$(INTDIR)/parser_pratt.obj" \
	".\Release\libsiod.lib"

"$(OUTDIR)\parser_pratt.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "parser_pratt - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "parser_pratt\Debug"
# PROP BASE Intermediate_Dir "parser_pratt\Debug"
# PROP BASE Target_Dir "parser_pratt"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir "parser_pratt"
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "libsiod - Win32 Debug" "$(OUTDIR)\parser_pratt.dll"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\parser_pratt.dll"
	-@erase ".\Debug\parser_pratt.obj"
	-@erase ".\Debug\parser_pratt.ilk"
	-@erase ".\Debug\parser_pratt.lib"
	-@erase ".\Debug\parser_pratt.exp"
	-@erase ".\Debug\parser_pratt.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/parser_pratt.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/parser_pratt.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/parser_pratt.pdb" /debug /machine:I386\
 /def:".\parser_pratt.def" /out:"$(OUTDIR)/parser_pratt.dll"\
 /implib:"$(OUTDIR)/parser_pratt.lib" 
DEF_FILE= \
	".\parser_pratt.def"
LINK32_OBJS= \
	"$(INTDIR)/parser_pratt.obj" \
	".\Debug\libsiod.lib"

"$(OUTDIR)\parser_pratt.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "tar - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "tar\Release"
# PROP BASE Intermediate_Dir "tar\Release"
# PROP BASE Target_Dir "tar"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir "tar"
OUTDIR=.\Release
INTDIR=.\Release

ALL : "libsiod - Win32 Release" "$(OUTDIR)\tar.dll"

CLEAN : 
	-@erase ".\Release\tar.dll"
	-@erase ".\Release\tar.obj"
	-@erase ".\Release\tar.lib"
	-@erase ".\Release\tar.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/tar.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/tar.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/tar.pdb" /machine:I386 /def:".\tar.def"\
 /out:"$(OUTDIR)/tar.dll" /implib:"$(OUTDIR)/tar.lib" 
DEF_FILE= \
	".\tar.def"
LINK32_OBJS= \
	"$(INTDIR)/tar.obj" \
	".\Release\libsiod.lib"

"$(OUTDIR)\tar.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "tar - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tar\Debug"
# PROP BASE Intermediate_Dir "tar\Debug"
# PROP BASE Target_Dir "tar"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir "tar"
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "libsiod - Win32 Debug" "$(OUTDIR)\tar.dll"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\tar.dll"
	-@erase ".\Debug\tar.obj"
	-@erase ".\Debug\tar.ilk"
	-@erase ".\Debug\tar.lib"
	-@erase ".\Debug\tar.exp"
	-@erase ".\Debug\tar.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/tar.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/tar.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/tar.pdb" /debug /machine:I386 /def:".\tar.def"\
 /out:"$(OUTDIR)/tar.dll" /implib:"$(OUTDIR)/tar.lib" 
DEF_FILE= \
	".\tar.def"
LINK32_OBJS= \
	"$(INTDIR)/tar.obj" \
	".\Debug\libsiod.lib"

"$(OUTDIR)\tar.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ss - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ss\Release"
# PROP BASE Intermediate_Dir "ss\Release"
# PROP BASE Target_Dir "ss"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir "ss"
OUTDIR=.\Release
INTDIR=.\Release

ALL : "libsiod - Win32 Release" "$(OUTDIR)\ss.dll"

CLEAN : 
	-@erase ".\Release\ss.dll"
	-@erase ".\Release\ss.obj"
	-@erase ".\Release\ss.lib"
	-@erase ".\Release\ss.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/ss.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ss.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/ss.pdb" /machine:I386 /def:".\ss.def" /out:"$(OUTDIR)/ss.dll"\
 /implib:"$(OUTDIR)/ss.lib" 
DEF_FILE= \
	".\ss.def"
LINK32_OBJS= \
	"$(INTDIR)/ss.obj" \
	"..\..\Lib\Wsock32.lib" \
	".\Release\libsiod.lib"

"$(OUTDIR)\ss.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ss - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ss\Debug"
# PROP BASE Intermediate_Dir "ss\Debug"
# PROP BASE Target_Dir "ss"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir "ss"
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "libsiod - Win32 Debug" "$(OUTDIR)\ss.dll"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\ss.dll"
	-@erase ".\Debug\ss.obj"
	-@erase ".\Debug\ss.ilk"
	-@erase ".\Debug\ss.lib"
	-@erase ".\Debug\ss.exp"
	-@erase ".\Debug\ss.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/ss.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ss.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/ss.pdb" /debug /machine:I386 /def:".\ss.def"\
 /out:"$(OUTDIR)/ss.dll" /implib:"$(OUTDIR)/ss.lib" 
DEF_FILE= \
	".\ss.def"
LINK32_OBJS= \
	"$(INTDIR)/ss.obj" \
	"..\..\Lib\Wsock32.lib" \
	".\Debug\libsiod.lib"

"$(OUTDIR)\ss.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "regex - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "regex\Release"
# PROP BASE Intermediate_Dir "regex\Release"
# PROP BASE Target_Dir "regex"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir "regex"
OUTDIR=.\Release
INTDIR=.\Release

ALL : "libsiod - Win32 Release" "$(OUTDIR)\regex.dll"

CLEAN : 
	-@erase ".\Release\regex.dll"
	-@erase ".\Release\regexec.obj"
	-@erase ".\Release\regcomp.obj"
	-@erase ".\Release\regex.obj"
	-@erase ".\Release\regfree.obj"
	-@erase ".\Release\regerror.obj"
	-@erase ".\Release\regex.lib"
	-@erase ".\Release\regex.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/regex.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/regex.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/regex.pdb" /machine:I386 /def:".\regex.def"\
 /out:"$(OUTDIR)/regex.dll" /implib:"$(OUTDIR)/regex.lib" 
DEF_FILE= \
	".\regex.def"
LINK32_OBJS= \
	"$(INTDIR)/regexec.obj" \
	"$(INTDIR)/regcomp.obj" \
	"$(INTDIR)/regex.obj" \
	"$(INTDIR)/regfree.obj" \
	"$(INTDIR)/regerror.obj" \
	".\Release\libsiod.lib"

"$(OUTDIR)\regex.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "regex - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "regex\Debug"
# PROP BASE Intermediate_Dir "regex\Debug"
# PROP BASE Target_Dir "regex"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir "regex"
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "libsiod - Win32 Debug" "$(OUTDIR)\regex.dll"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\regex.dll"
	-@erase ".\Debug\regcomp.obj"
	-@erase ".\Debug\regfree.obj"
	-@erase ".\Debug\regerror.obj"
	-@erase ".\Debug\regex.obj"
	-@erase ".\Debug\regexec.obj"
	-@erase ".\Debug\regex.ilk"
	-@erase ".\Debug\regex.lib"
	-@erase ".\Debug\regex.exp"
	-@erase ".\Debug\regex.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/regex.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/regex.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/regex.pdb" /debug /machine:I386 /def:".\regex.def"\
 /out:"$(OUTDIR)/regex.dll" /implib:"$(OUTDIR)/regex.lib" 
DEF_FILE= \
	".\regex.def"
LINK32_OBJS= \
	"$(INTDIR)/regcomp.obj" \
	"$(INTDIR)/regfree.obj" \
	"$(INTDIR)/regerror.obj" \
	"$(INTDIR)/regex.obj" \
	"$(INTDIR)/regexec.obj" \
	".\Debug\libsiod.lib"

"$(OUTDIR)\regex.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dummy - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "dummy\Release"
# PROP BASE Intermediate_Dir "dummy\Release"
# PROP BASE Target_Dir "dummy"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir "dummy"
OUTDIR=.\Release
INTDIR=.\Release

ALL : "libsiod - Win32 Release" "tar - Win32 Release" "ss - Win32 Release"\
 "regex - Win32 Release" "parser_pratt - Win32 Release" "$(OUTDIR)\dummy.exe"

CLEAN : 
	-@erase ".\Release\dummy.exe"
	-@erase ".\Release\sample.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/dummy.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=

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

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dummy.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/dummy.pdb" /machine:I386 /out:"$(OUTDIR)/dummy.exe" 
LINK32_OBJS= \
	"$(INTDIR)/sample.obj" \
	".\Release\parser_pratt.lib" \
	".\Release\regex.lib" \
	".\Release\ss.lib" \
	".\Release\tar.lib" \
	".\Release\libsiod.lib"

"$(OUTDIR)\dummy.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dummy - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "dummy\Debug"
# PROP BASE Intermediate_Dir "dummy\Debug"
# PROP BASE Target_Dir "dummy"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir "dummy"
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "libsiod - Win32 Debug" "tar - Win32 Debug" "ss - Win32 Debug"\
 "regex - Win32 Debug" "parser_pratt - Win32 Debug" "$(OUTDIR)\dummy.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\dummy.exe"
	-@erase ".\Debug\sample.obj"
	-@erase ".\Debug\dummy.ilk"
	-@erase ".\Debug\dummy.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/dummy.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=

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

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dummy.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/dummy.pdb" /debug /machine:I386 /out:"$(OUTDIR)/dummy.exe" 
LINK32_OBJS= \
	"$(INTDIR)/sample.obj" \
	".\Debug\parser_pratt.lib" \
	".\Debug\regex.lib" \
	".\Debug\ss.lib" \
	".\Debug\tar.lib" \
	".\Debug\libsiod.lib"

"$(OUTDIR)\dummy.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wcsiod - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wcsiod\Release"
# PROP BASE Intermediate_Dir "wcsiod\Release"
# PROP BASE Target_Dir "wcsiod"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir "wcsiod"
OUTDIR=.\Release
INTDIR=.\Release

ALL : "libsiod - Win32 Release" "$(OUTDIR)\wcsiod.exe"

CLEAN : 
	-@erase ".\Release\wcsiod.exe"
	-@erase ".\Release\wcsiod.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/wcsiod.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=

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

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wcsiod.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/wcsiod.pdb" /machine:I386 /out:"$(OUTDIR)/wcsiod.exe" 
LINK32_OBJS= \
	"$(INTDIR)/wcsiod.obj" \
	".\Release\libsiod.lib"

"$(OUTDIR)\wcsiod.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wcsiod - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wcsiod\Debug"
# PROP BASE Intermediate_Dir "wcsiod\Debug"
# PROP BASE Target_Dir "wcsiod"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir "wcsiod"
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "libsiod - Win32 Debug" "$(OUTDIR)\wcsiod.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\wcsiod.exe"
	-@erase ".\Debug\wcsiod.obj"
	-@erase ".\Debug\wcsiod.ilk"
	-@erase ".\Debug\wcsiod.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/wcsiod.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=

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

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wcsiod.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/wcsiod.pdb" /debug /machine:I386 /out:"$(OUTDIR)/wcsiod.exe" 
LINK32_OBJS= \
	"$(INTDIR)/wcsiod.obj" \
	".\Debug\libsiod.lib"

"$(OUTDIR)\wcsiod.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "siod - Win32 Release"
# Name "siod - Win32 Debug"

!IF  "$(CFG)" == "siod - Win32 Release"

!ELSEIF  "$(CFG)" == "siod - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\siod.c
DEP_CPP_SIOD_=\
	".\siod.h"\
	

"$(INTDIR)\siod.obj" : $(SOURCE) $(DEP_CPP_SIOD_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "libsiod"

!IF  "$(CFG)" == "siod - Win32 Release"

"libsiod - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\siod.mak CFG="libsiod - Win32 Release" 

!ELSEIF  "$(CFG)" == "siod - Win32 Debug"

"libsiod - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\siod.mak CFG="libsiod - Win32 Debug" 

!ENDIF 

# End Project Dependency
# End Target
################################################################################
# Begin Target

# Name "libsiod - Win32 Release"
# Name "libsiod - Win32 Debug"

!IF  "$(CFG)" == "libsiod - Win32 Release"

!ELSEIF  "$(CFG)" == "libsiod - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\md5.c

!IF  "$(CFG)" == "libsiod - Win32 Release"

DEP_CPP_MD5_C=\
	".\md5.h"\
	

"$(INTDIR)\md5.obj" : $(SOURCE) $(DEP_CPP_MD5_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libsiod - Win32 Debug"

DEP_CPP_MD5_C=\
	".\md5.h"\
	

"$(INTDIR)\md5.obj" : $(SOURCE) $(DEP_CPP_MD5_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\slibu.c

!IF  "$(CFG)" == "libsiod - Win32 Release"

DEP_CPP_SLIBU=\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\siod.h"\
	".\siodp.h"\
	".\md5.h"\
	

"$(INTDIR)\slibu.obj" : $(SOURCE) $(DEP_CPP_SLIBU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libsiod - Win32 Debug"

DEP_CPP_SLIBU=\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\siod.h"\
	".\siodp.h"\
	".\md5.h"\
	

"$(INTDIR)\slibu.obj" : $(SOURCE) $(DEP_CPP_SLIBU) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sliba.c

!IF  "$(CFG)" == "libsiod - Win32 Release"

DEP_CPP_SLIBA=\
	".\siod.h"\
	".\siodp.h"\
	

"$(INTDIR)\sliba.obj" : $(SOURCE) $(DEP_CPP_SLIBA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libsiod - Win32 Debug"

DEP_CPP_SLIBA=\
	".\siod.h"\
	".\siodp.h"\
	

"$(INTDIR)\sliba.obj" : $(SOURCE) $(DEP_CPP_SLIBA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\slib.c

!IF  "$(CFG)" == "libsiod - Win32 Release"

DEP_CPP_SLIB_=\
	".\siod.h"\
	".\siodp.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Timeb.h"\
	

"$(INTDIR)\slib.obj" : $(SOURCE) $(DEP_CPP_SLIB_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libsiod - Win32 Debug"

DEP_CPP_SLIB_=\
	".\siod.h"\
	".\siodp.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Timeb.h"\
	

"$(INTDIR)\slib.obj" : $(SOURCE) $(DEP_CPP_SLIB_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\trace.c

!IF  "$(CFG)" == "libsiod - Win32 Release"

DEP_CPP_TRACE=\
	".\siod.h"\
	".\siodp.h"\
	

"$(INTDIR)\trace.obj" : $(SOURCE) $(DEP_CPP_TRACE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libsiod - Win32 Debug"

DEP_CPP_TRACE=\
	".\siod.h"\
	".\siodp.h"\
	

"$(INTDIR)\trace.obj" : $(SOURCE) $(DEP_CPP_TRACE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\libsiod.def

!IF  "$(CFG)" == "libsiod - Win32 Release"

!ELSEIF  "$(CFG)" == "libsiod - Win32 Debug"

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "parser_pratt - Win32 Release"
# Name "parser_pratt - Win32 Debug"

!IF  "$(CFG)" == "parser_pratt - Win32 Release"

!ELSEIF  "$(CFG)" == "parser_pratt - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "libsiod"

!IF  "$(CFG)" == "parser_pratt - Win32 Release"

"libsiod - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\siod.mak CFG="libsiod - Win32 Release" 

!ELSEIF  "$(CFG)" == "parser_pratt - Win32 Debug"

"libsiod - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\siod.mak CFG="libsiod - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\parser_pratt.c

!IF  "$(CFG)" == "parser_pratt - Win32 Release"

DEP_CPP_PARSE=\
	".\siod.h"\
	

"$(INTDIR)\parser_pratt.obj" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "parser_pratt - Win32 Debug"

DEP_CPP_PARSE=\
	".\siod.h"\
	

"$(INTDIR)\parser_pratt.obj" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\parser_pratt.def

!IF  "$(CFG)" == "parser_pratt - Win32 Release"

!ELSEIF  "$(CFG)" == "parser_pratt - Win32 Debug"

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "tar - Win32 Release"
# Name "tar - Win32 Debug"

!IF  "$(CFG)" == "tar - Win32 Release"

!ELSEIF  "$(CFG)" == "tar - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "libsiod"

!IF  "$(CFG)" == "tar - Win32 Release"

"libsiod - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\siod.mak CFG="libsiod - Win32 Release" 

!ELSEIF  "$(CFG)" == "tar - Win32 Debug"

"libsiod - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\siod.mak CFG="libsiod - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\tar.c

!IF  "$(CFG)" == "tar - Win32 Release"

DEP_CPP_TAR_C=\
	".\siod.h"\
	

"$(INTDIR)\tar.obj" : $(SOURCE) $(DEP_CPP_TAR_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "tar - Win32 Debug"

DEP_CPP_TAR_C=\
	".\siod.h"\
	

"$(INTDIR)\tar.obj" : $(SOURCE) $(DEP_CPP_TAR_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tar.def

!IF  "$(CFG)" == "tar - Win32 Release"

!ELSEIF  "$(CFG)" == "tar - Win32 Debug"

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "ss - Win32 Release"
# Name "ss - Win32 Debug"

!IF  "$(CFG)" == "ss - Win32 Release"

!ELSEIF  "$(CFG)" == "ss - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "libsiod"

!IF  "$(CFG)" == "ss - Win32 Release"

"libsiod - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\siod.mak CFG="libsiod - Win32 Release" 

!ELSEIF  "$(CFG)" == "ss - Win32 Debug"

"libsiod - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\siod.mak CFG="libsiod - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\ss.c

!IF  "$(CFG)" == "ss - Win32 Release"

DEP_CPP_SS_C10=\
	{$(INCLUDE)}"\sys\Types.h"\
	".\siod.h"\
	".\ss.h"\
	

"$(INTDIR)\ss.obj" : $(SOURCE) $(DEP_CPP_SS_C10) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ss - Win32 Debug"

DEP_CPP_SS_C10=\
	{$(INCLUDE)}"\sys\Types.h"\
	".\siod.h"\
	".\ss.h"\
	

"$(INTDIR)\ss.obj" : $(SOURCE) $(DEP_CPP_SS_C10) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Msdevstd\Lib\Wsock32.lib

!IF  "$(CFG)" == "ss - Win32 Release"

!ELSEIF  "$(CFG)" == "ss - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ss.def

!IF  "$(CFG)" == "ss - Win32 Release"

!ELSEIF  "$(CFG)" == "ss - Win32 Debug"

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "regex - Win32 Release"
# Name "regex - Win32 Debug"

!IF  "$(CFG)" == "regex - Win32 Release"

!ELSEIF  "$(CFG)" == "regex - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\regex.c

!IF  "$(CFG)" == "regex - Win32 Release"

DEP_CPP_REGEX=\
	{$(INCLUDE)}"\regex.h"\
	".\siod.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	

"$(INTDIR)\regex.obj" : $(SOURCE) $(DEP_CPP_REGEX) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "regex - Win32 Debug"

DEP_CPP_REGEX=\
	{$(INCLUDE)}"\regex.h"\
	".\siod.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	

"$(INTDIR)\regex.obj" : $(SOURCE) $(DEP_CPP_REGEX) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\regex.def

!IF  "$(CFG)" == "regex - Win32 Release"

!ELSEIF  "$(CFG)" == "regex - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "libsiod"

!IF  "$(CFG)" == "regex - Win32 Release"

"libsiod - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\siod.mak CFG="libsiod - Win32 Release" 

!ELSEIF  "$(CFG)" == "regex - Win32 Debug"

"libsiod - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\siod.mak CFG="libsiod - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\regcomp.c

!IF  "$(CFG)" == "regex - Win32 Release"

DEP_CPP_REGCO=\
	{$(INCLUDE)}"\sys\Types.h"\
	".\regex.h"\
	".\utils.h"\
	".\regex2.h"\
	".\cclass.h"\
	".\cname.h"\
	

"$(INTDIR)\regcomp.obj" : $(SOURCE) $(DEP_CPP_REGCO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "regex - Win32 Debug"

DEP_CPP_REGCO=\
	{$(INCLUDE)}"\sys\Types.h"\
	".\regex.h"\
	".\utils.h"\
	".\regex2.h"\
	".\cclass.h"\
	".\cname.h"\
	

"$(INTDIR)\regcomp.obj" : $(SOURCE) $(DEP_CPP_REGCO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\regerror.c

!IF  "$(CFG)" == "regex - Win32 Release"

DEP_CPP_REGER=\
	{$(INCLUDE)}"\sys\Types.h"\
	".\regex.h"\
	".\utils.h"\
	

"$(INTDIR)\regerror.obj" : $(SOURCE) $(DEP_CPP_REGER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "regex - Win32 Debug"

DEP_CPP_REGER=\
	{$(INCLUDE)}"\sys\Types.h"\
	".\regex.h"\
	".\utils.h"\
	

"$(INTDIR)\regerror.obj" : $(SOURCE) $(DEP_CPP_REGER) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\regfree.c

!IF  "$(CFG)" == "regex - Win32 Release"

DEP_CPP_REGFR=\
	{$(INCLUDE)}"\sys\Types.h"\
	".\regex.h"\
	".\utils.h"\
	".\regex2.h"\
	

"$(INTDIR)\regfree.obj" : $(SOURCE) $(DEP_CPP_REGFR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "regex - Win32 Debug"

DEP_CPP_REGFR=\
	{$(INCLUDE)}"\sys\Types.h"\
	".\regex.h"\
	".\utils.h"\
	".\regex2.h"\
	

"$(INTDIR)\regfree.obj" : $(SOURCE) $(DEP_CPP_REGFR) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\regexec.c

!IF  "$(CFG)" == "regex - Win32 Release"

DEP_CPP_REGEXE=\
	{$(INCLUDE)}"\sys\Types.h"\
	".\regex.h"\
	".\utils.h"\
	".\regex2.h"\
	".\engine.c"\
	

"$(INTDIR)\regexec.obj" : $(SOURCE) $(DEP_CPP_REGEXE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "regex - Win32 Debug"

DEP_CPP_REGEXE=\
	{$(INCLUDE)}"\sys\Types.h"\
	".\regex.h"\
	".\utils.h"\
	".\regex2.h"\
	".\engine.c"\
	

"$(INTDIR)\regexec.obj" : $(SOURCE) $(DEP_CPP_REGEXE) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "dummy - Win32 Release"
# Name "dummy - Win32 Debug"

!IF  "$(CFG)" == "dummy - Win32 Release"

!ELSEIF  "$(CFG)" == "dummy - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "parser_pratt"

!IF  "$(CFG)" == "dummy - Win32 Release"

"parser_pratt - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\siod.mak CFG="parser_pratt - Win32 Release" 

!ELSEIF  "$(CFG)" == "dummy - Win32 Debug"

"parser_pratt - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\siod.mak CFG="parser_pratt - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "regex"

!IF  "$(CFG)" == "dummy - Win32 Release"

"regex - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\siod.mak CFG="regex - Win32 Release" 

!ELSEIF  "$(CFG)" == "dummy - Win32 Debug"

"regex - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\siod.mak CFG="regex - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "ss"

!IF  "$(CFG)" == "dummy - Win32 Release"

"ss - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\siod.mak CFG="ss - Win32 Release" 

!ELSEIF  "$(CFG)" == "dummy - Win32 Debug"

"ss - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\siod.mak CFG="ss - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "tar"

!IF  "$(CFG)" == "dummy - Win32 Release"

"tar - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\siod.mak CFG="tar - Win32 Release" 

!ELSEIF  "$(CFG)" == "dummy - Win32 Debug"

"tar - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\siod.mak CFG="tar - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\sample.c

!IF  "$(CFG)" == "dummy - Win32 Release"

DEP_CPP_SAMPL=\
	".\siod.h"\
	

"$(INTDIR)\sample.obj" : $(SOURCE) $(DEP_CPP_SAMPL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dummy - Win32 Debug"

DEP_CPP_SAMPL=\
	".\siod.h"\
	

"$(INTDIR)\sample.obj" : $(SOURCE) $(DEP_CPP_SAMPL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "libsiod"

!IF  "$(CFG)" == "dummy - Win32 Release"

"libsiod - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\siod.mak CFG="libsiod - Win32 Release" 

!ELSEIF  "$(CFG)" == "dummy - Win32 Debug"

"libsiod - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\siod.mak CFG="libsiod - Win32 Debug" 

!ENDIF 

# End Project Dependency
# End Target
################################################################################
# Begin Target

# Name "wcsiod - Win32 Release"
# Name "wcsiod - Win32 Debug"

!IF  "$(CFG)" == "wcsiod - Win32 Release"

!ELSEIF  "$(CFG)" == "wcsiod - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\wcsiod.c

!IF  "$(CFG)" == "wcsiod - Win32 Release"

DEP_CPP_WCSIO=\
	".\siod.h"\
	

"$(INTDIR)\wcsiod.obj" : $(SOURCE) $(DEP_CPP_WCSIO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "wcsiod - Win32 Debug"

DEP_CPP_WCSIO=\
	".\siod.h"\
	

"$(INTDIR)\wcsiod.obj" : $(SOURCE) $(DEP_CPP_WCSIO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "libsiod"

!IF  "$(CFG)" == "wcsiod - Win32 Release"

"libsiod - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\siod.mak CFG="libsiod - Win32 Release" 

!ELSEIF  "$(CFG)" == "wcsiod - Win32 Debug"

"libsiod - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\siod.mak CFG="libsiod - Win32 Debug" 

!ENDIF 

# End Project Dependency
# End Target
# End Project
################################################################################
