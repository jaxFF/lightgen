@echo off
setlocal

REM Usage: build.bat <debug|release|clean> <msvc|clang>

set ProjectDir=%~dp0
set BuildDir=%~dp0build

set BuildType=%1
set Compiler=%2

IF NOT "%1"=="" ( 
    IF "%BuildType%"=="msvc" (
        set Compiler=%BuildType%
        set BuildType=
    )

    IF "%BuildType%"=="clang" (
        set Compiler=%BuildType%
        set BuildType=
    )
)

IF "%BuildType%"=="" (set BuildType=debug)
IF "%Compiler%"=="" (set Compiler=msvc)
IF "%BuildType%"=="clean" (goto clean)


set CommonWarningsMSVC=-wd4302 -wd4311 -wd4201 -wd4100 -wd4189 -wd4505 -wd4164 -wd4127 -wd4018 -wd4244 -wd4146 -wd4700 -wd4996
set CommonWarningsClang=-Wno-writable-strings -Wno-unused-variable -Wno-pointer-to-int-cast -Wno-pointer-bool-conversion -Wno-unused-command-line-argument -Wno-constant-logical-operand -Wno-unknown-pragmas -Wno-missing-braces -Wno-varargs -Wno-unused-function -Wno-ignored-attributes %CommonWarningsMSVC%
set CommonCompilerFlags=-DPLATFORM_WINDOWS=1 -fp:fast -fp:except- -nologo -Gm- -GR- -EHsc- -EHa- -MP -Od -Oi -FC -Zi -GS- -Gs9999999 -WX -W3
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib kernel32.lib shlwapi.lib

IF "%BuildType%"=="release" (
    set CommonCompilerFlags=%CommonCompilerFlags% -O2
)

IF "%Compiler%"=="msvc" (
    echo Building with MSVC in %BuildType%
    set CompileCommand=cl
    set CommonCompilerFlags=-DCOMPILER_MSVC=1 %CommonCompilerFlags% %CommonWarningsMSVC%
) ELSE IF "%Compiler%"=="clang" (
    echo Building with Clang in %BuildType%
    set CompileCommand=clang-cl
    set CommonCompilerFlags=-DCOMPILER_CLANG=1 %CommonCompilerFlags% %CommonWarningsClang%
)

IF NOT EXIST %BuildDir% mkdir %BuildDir%
pushd %BuildDir%
del *.pdb > NUL 2> NUL
del *.map > NUL 2> NUL

echo Compilation flags:%CommonCompilerFlags% /link%CommonLinkerFlags%

REM 64-bit generator build
%CompileCommand% %CommonCompilerFlags% ..\src\lightgen.cpp /link %CommonLinkerFlags%

exit /b 0

:clean
echo Cleaning build directory: %BuildDir% 
echo Are you sure you want to continue [Y/n]? *ALL FILES WILL BE DELETED!!!*
set /P c=
if /I "%c%"=="N" (exit /b 0) else (
    echo Removing files in %BuildDir%...
    del /s /q %BuildDir%\*.*
)

exit /b 0