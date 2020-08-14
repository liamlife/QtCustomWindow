@echo off

echo _____________________________________________________
echo    Start build Window x64 Debug project.
echo _____________________________________________________


set currPath=%~dp0
set parentPath=%cd%

echo %currPath%
echo %parentPath%

pushd %1 & for %%i in (.) do set curr=%%~ni
echo %curr%

set buildPath=D:\temp\%curr%\build_debug\
set checkFile=%buildPath%robot_client.sln

if exist %buildPath% (
	echo "%buildPath% is exist."
) else (
	md "%buildPath%
	echo "%buildPath% not exist, create..."
)

cd %buildPath%

if exist %checkFile% (
	echo "%checkFile% is exist."
) else (
	echo "%checkFile% not exist, create..."
	cmake -G "Visual Studio 15 2017 Win64" %parentPath%
)


cmake --build .

pause

start qt_custom_window.sln
