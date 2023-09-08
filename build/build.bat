@echo off

rem The "Safe" name doesn't have spaces or invalid characters and also doesn't change for Demo vs non-Demo
set ProjectName=Game 1
set ProjectNameSafe=Game1

set DebugBuild=1
set DeveloperBuild=1
set DemoBuild=0
set AssertionsEnabled=1

set LibDirectory=..\lib
set SourceDirectory=..\code_c
set DataDirectory=..\data
set PlaydateSdkDirectory=C:\Users\robbitay\Documents\MyStuff\Programs\PlaydateSDK
set SimCompilerExeName=gcc
set ArmCompilerExeName=arm-none-eabi-gcc
set PdcExeName=%PlaydateSdkDirectory%\bin\pdc
set MainSourcePath=%SourceDirectory%\main.cpp
set OutputObjName=%ProjectNameSafe%.obj
set OutputLibName=pdex.lib
set OutputDllName=pdex.dll
set OutputPdbName=pdex.pdb
set PdcOutputFolder=%ProjectNameSafe%.pdx
set IncVersNumScriptPath=..\IncrementVersionNumber.py
set VersionFilePath=%SourceDirectory%\version.h
set TimeString=%date:~-4,4%%date:~-10,2%%date:~-7,2%%time:~0,2%%time:~3,2%%time:~6,2%
set OutputObjPdbName=%ProjectNameSafe%_obj_%TimeString%.pdb

echo Running on %ComputerName%

if "%DemoBuild%"=="1" (
	set ProjectName=%ProjectName% Demo
)

python --version 2>NUL
if errorlevel 1 (
	echo WARNING: Python isn't installed on this computer. The build number will not be incremented
	set PythonInstalled=0
) else (
	set PythonInstalled=1
)

rem call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
rem call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64 -no_logo
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64

set CompilerFlags=/GS /Zi /Gm- /Od /RTC1 /std:c++20 /Gd /MDd /Zc:wchar_t /Ob0 /Zc:inline /fp:precise /W3 /WX- /nologo
set CompilerFlags=%CompilerFlags% /Fd"%OutputObjPdbName%" /Fp"%ProjectNameSafe%.pch"
set CompilerFlags=%CompilerFlags% /D "PLAYDATE_COMPILATION" /D "C_ONLY_COMPILATION" /D "PROJECT_NAME=\"%ProjectName%\"" /D "PROJECT_NAME_SAFE=\"%ProjectNameSafe%\"" /D "DEBUG_BUILD=%DebugBuild%" /D "DEVELOPER_BUILD=%DeveloperBuild%" /D "DEMO_BUILD=%DemoBuild%" /D "STEAM_BUILD=0" /D "PROCMON_SUPPORTED=0" /D "SOCKETS_SUPPORTED=0" /D "BOX2D_SUPPORTED=0" /D "OPENGL_SUPPORTED=0" /D "VULKAN_SUPPORTED=0" /D "DIRECTX_SUPPORTED=0" /D "SLUG_SUPPORTED=0" /D "JSON_SUPPORTED=0" /D "ASSERTIONS_ENABLED=%AssertionsEnabled%"
set CompilerFlags=%CompilerFlags% /D "_WINDLL" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "TARGET_SIMULATOR=1" /D "_WINDLL=1" /D "TARGET_EXTENSION=1"
set CompilerFlags=%CompilerFlags% /errorReport:prompt /Zc:forScope /diagnostics:column
set IncludeDirectories=/I"%SourceDirectory%" /I"%LibDirectory%\include" /I"%PlaydateSdkDirectory%\C_API"
set LinkerFlags=/MANIFEST /NXCOMPAT /DYNAMICBASE /DEBUG /DLL /MACHINE:X64 /INCREMENTAL /SUBSYSTEM:CONSOLE /ERRORREPORT:PROMPT /NOLOGO /TLBID:1
set LinkerFlags=%LinkerFlags% /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /ManifestFile:"%OutputDllName%.intermediate.manifest" /LTCGOUT:"%ProjectNameSafe%.iobj" /ILK:"%ProjectNameSafe%.ilk"
set Libraries="kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "comdlg32.lib" "advapi32.lib"
set PdcFlags=-k -sdkpath "%PlaydateSdkDirectory%"

if "%PythonInstalled%"=="1" (
	python %IncVersNumScriptPath% %VersionFilePath%
)

del *.pdb > NUL 2> NUL
del *.obj > NUL 2> NUL

cl /Fo"%OutputObjName%" %CompilerFlags% %IncludeDirectories% /c "%MainSourcePath%"
LINK %LinkerFlags% %Libraries% "%OutputObjName%" /OUT:"%OutputDllName%" /IMPLIB:"%OutputLibName%" /PDB:"%OutputPdbName%"
XCOPY ".\%OutputDllName%" "%DataDirectory%\" /Y > NUL

%PdcExeName% %PdcFlags% "%DataDirectory%" "%PdcOutputFolder%"
