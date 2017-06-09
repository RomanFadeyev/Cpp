
MTDevps.dll: dlldata.obj MTDev_p.obj MTDev_i.obj
	link /dll /out:MTDevps.dll /def:MTDevps.def /entry:DllMain dlldata.obj MTDev_p.obj MTDev_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \
.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del MTDevps.dll
	@del MTDevps.lib
	@del MTDevps.exp
	@del dlldata.obj
	@del MTDev_p.obj
	@del MTDev_i.obj

MTDevps.dll: dlldata.obj MTDev_p.obj MTDev_i.obj
	link /dll /out:MTDevps.dll /def:MTDevps.def /entry:DllMain dlldata.obj MTDev_p.obj MTDev_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \
.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del MTDevps.dll
	@del MTDevps.lib
	@del MTDevps.exp
	@del dlldata.obj
	@del MTDev_p.obj
	@del MTDev_i.obj
