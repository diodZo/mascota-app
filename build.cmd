@echo off
setlocal
cd /d "%~dp0"
where cl >nul 2>nul
if not errorlevel 1 goto ready
for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do call "%%i\VC\Auxiliary\Build\vcvars64.bat"
:ready
if not exist build mkdir build
cl /nologo /std:c++17 /EHsc /W4 /WX /utf-8 /Fo:build\tests.obj tests\core_tests.cpp /Fe:build\core_tests.exe
if errorlevel 1 exit /b 1
build\core_tests.exe
if errorlevel 1 exit /b 1
cl /nologo /std:c++17 /EHsc /W4 /WX /utf-8 /Fo:build\evolution_tests.obj tests\evolution_tests.cpp /Fe:build\evolution_tests.exe
if errorlevel 1 exit /b 1
build\evolution_tests.exe
if errorlevel 1 exit /b 1
cl /nologo /std:c++17 /EHsc /W4 /WX /utf-8 /Fo:build\multispecies_tests.obj tests\multispecies_tests.cpp /Fe:build\multispecies_tests.exe
if errorlevel 1 exit /b 1
build\multispecies_tests.exe
if errorlevel 1 exit /b 1
cl /nologo /std:c++17 /EHsc /W4 /WX /utf-8 /Fo:build\desktop.obj platforms\desktop\main.cpp /Fe:build\virtual_pet.exe /link /SUBSYSTEM:WINDOWS user32.lib gdi32.lib windowscodecs.lib ole32.lib msimg32.lib shell32.lib
if errorlevel 1 exit /b 1
cl /nologo /std:c++17 /EHsc /W4 /WX /utf-8 /Fo:build\assets_tests.obj tests\assets_tests.cpp /Fe:build\assets_tests.exe /link user32.lib gdi32.lib windowscodecs.lib ole32.lib msimg32.lib
if errorlevel 1 exit /b 1
build\assets_tests.exe "%CD%"
exit /b %errorlevel%
