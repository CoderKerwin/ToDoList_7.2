ECHO OFF

if "%~1" == "" exit

set REPOROOT=C:\Users\Daniel.Godson\Documents\GitHub

if NOT EXIST %REPOROOT% set REPOROOT=D:\_code
if NOT EXIST %REPOROOT% exit

set LATESTREPO="%~1"
set RESREPO=%REPOROOT%\ToDoList_Resources

ECHO LATESTREPO=%LATESTREPO%
ECHO RESREPO=%RESREPO%

REM 7-Zip Location
set PATH7ZIP="C:\Program Files (x86)\7-Zip\7z.exe"

if NOT EXIST %PATH7ZIP% set PATH7ZIP="C:\Program Files\7-Zip\7z.exe"
if NOT EXIST %PATH7ZIP% exit

ECHO PATH7ZIP=%PATH7ZIP%

set OUTDIR=%1\ToDoList\Unicode_Release
set OUTZIP=%OUTDIR%\todolist_exe_.zip

ECHO OUTDIR=%OUTDIR%
ECHO OUTZIP=%OUTZIP%

del %OUTZIP%

REM Zip C++ Binaries
%PATH7ZIP% a %OUTZIP% %OUTDIR%\ToDoList.exe
%PATH7ZIP% a %OUTZIP% %OUTDIR%\TDLUpdate.exe
%PATH7ZIP% a %OUTZIP% %OUTDIR%\TDLUninstall.exe
%PATH7ZIP% a %OUTZIP% %OUTDIR%\TDLTransEdit.exe

REM Manifest for XP only (Updater will delete for other OSes)
%PATH7ZIP% a %OUTZIP% %1\ToDoList\res\ToDoList.exe.XP.manifest

REM Handle dlls explicitly to maintain control over plugins
%PATH7ZIP% a %OUTZIP% %OUTDIR%\PlainTextImport.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\Rtf2HtmlBridge.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\RTFContentCtrl.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\BurndownExt.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\TransText.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\CalendarExt.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\EncryptDecrypt.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\FMindImportExport.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\FtpStorage.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\GanttChartExt.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\GPExport.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\iCalImportExport.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\Itenso.*.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\KanbanBoard.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\MLOImport.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\MySpellCheck.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\OdbcStorage.dll

REM .Net Plugins
%PATH7ZIP% a %OUTZIP% %OUTDIR%\PluginHelpers.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\DayViewUIExtensionCore.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\DayViewUIExtensionBridge.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\MindMapUIExtensionCore.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\MindMapUIExtensionBridge.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\Calendar.DayView.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\WordCloudUIExtensionCore.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\WordCloudUIExtensionBridge.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\Gma.CodeCloud.Controls.dll

REM ToDoList_Dev-only Plugins
echo.%1 | findstr /C:"_Dev" 1>nul

IF ERRORLEVEL 1 goto CONTINUE

%PATH7ZIP% a %OUTZIP% %OUTDIR%\WorkloadExt.dll
%PATH7ZIP% a %OUTZIP% %OUTDIR%\HTMLContentControlBridge.dll"
%PATH7ZIP% a %OUTZIP% %OUTDIR%\HTMLContentControlCore.dll"
%PATH7ZIP% a %OUTZIP% %OUTDIR%\MSDN.HtmlEditorControl.dll"
REM %PATH7ZIP% a %OUTZIP% %OUTDIR%\Microsoft.mshtml.dll"

:CONTINUE
 
REM Copy latest Resources
del %OUTDIR%\Resources\ /Q /S
del %OUTDIR%\Resources\Translations\backup\ /Q
xcopy %RESREPO%\*.* %OUTDIR%\Resources\ /Y /D /E /EXCLUDE:%LATESTREPO%\BuildReleaseZip_Exclude.txt

REM Zip install instructions to root
%PATH7ZIP% a %OUTZIP% %OUTDIR%\Resources\Install.Windows.txt
%PATH7ZIP% a %OUTZIP% %OUTDIR%\Resources\Install.Linux.txt

REM And remove from resources to avoid duplication
del %OUTDIR%\Resources\Install.Windows.txt
del %OUTDIR%\Resources\Install.Linux.txt

REM Zip Resources
%PATH7ZIP% a %OUTZIP% %OUTDIR%\Resources\ -x!.git*

REM Open zip for inspection
%OUTZIP%

pause
