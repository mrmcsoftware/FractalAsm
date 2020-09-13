ALL: fractald.exe fractalf.exe

CLEAN:
	-@erase fractald.obj
	-@erase fractalf.obj
	-@erase vc*.idb
	-@erase fractald.exe
	-@erase fractalf.exe
	-@erase fractal.res

CPP=cl.exe

# /ML or /MD - use whichever is faster on your machine and whichever
#              is supported by your compiler version

CPP_PROJ=/nologo /MD /W0 /EHsc /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c /D "_WIN32_WINNT=0x0500"

.c.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

fractal.res: fractal.rc
   rc -r fractal.rc

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib /nologo /subsystem:windows /incremental:no /machine:I386

OBJS_D=fractald.obj fractal.res
OBJS_F=fractalf.obj fractal.res

fractald.exe: $(OBJS_D)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(OBJS_D)
<<

fractalf.exe: $(OBJS_F)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(OBJS_F)
<<

fractald.obj: fractald.c
fractalf.obj: fractalf.c
