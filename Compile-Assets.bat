@echo off
setlocal

echo Choose build configuration:
echo 1. Debug
echo 2. Release
set /p configChoice=Enter 1 or 2: 

if "%configChoice%"=="1" (
	set config=Debug
) else if "%configChoice%"=="2" (
	set config=Release
) else (
	echo Invalid choice.
	exit /b 1
)

set engine_source=Source\Engine\Assets

set editor_source=Source\Editor\Assets
set editor_destination=Binaries\%config%-windows-x86_64\Assets

if not exist "%editor_destination%" (
	mkdir "%editor_destination%"
)

echo Moving Engine assets to Editor build
xcopy /E /I /Y "%engine_source%" "%editor_destination%"

echo Moving Editor assets to Editor build
xcopy /E /I /Y "%editor_source%" "%editor_destination%"

echo Checking Editor Shaders...
if exist "%editor_destination%\Shaders\Compile.bat" (
	echo Compiling Editor shaders
	pushd "%editor_destination%\Shaders"
	call Compile.bat
	popd
) else (
	echo Compile.bat not found in Editor Shaders folder!
)

echo Done.
pause
endlocal