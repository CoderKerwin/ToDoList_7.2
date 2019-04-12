ECHO OFF
CLS

set REPOROOT=C:\Users\Daniel.Godson\Documents\GitHub

if NOT EXIST %REPOROOT% set REPOROOT=D:\_code
if NOT EXIST %REPOROOT% exit

set LATESTREPO=%REPOROOT%\ToDoList_7.2
set RESREPO=%REPOROOT%\ToDoList_Resources
set PLUGINSREPO=%REPOROOT%\ToDoList_Plugins_7.2

ECHO REPOROOT=%REPOROOT%
ECHO LATESTREPO=%LATESTREPO%
ECHO RESREPO=%RESREPO%
ECHO PLUGINSREPO=%PLUGINSREPO%

ECHO ON

REM - Build Core App
cd %LATESTREPO%
"C:\Program Files (x86)\Microsoft Visual Studio\Common\MSDev98\Bin\msdev.exe" .\ToDoList\ToDoList_All.dsw /MAKE "ALL - Win32 Unicode Release" 

REM - Build Plugins
cd %PLUGINSREPO%
"C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE\devenv.com" .\ToDoList_Plugins.sln /Build "Release"

REM Allow caller to cancel
pause

CALL %LATESTREPO%\BuildReleaseZip.bat %LATESTREPO%