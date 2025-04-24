@echo off
setlocal EnableDelayedExpansion

:: Set the solution filename (must match the project name in CMakeLists.txt)
set "SLN_FILE=libnoise-modern.sln"

:: Default value for launching Visual Studio after build (OFF by default)
set "START_VS_AFTER_BUILD=OFF"

:: Check for command-line argument to override START_VS_AFTER_BUILD
if not "%1"=="" (
    set "START_VS_AFTER_BUILD=%1"
)

:: Validate START_VS_AFTER_BUILD value
if /I not "!START_VS_AFTER_BUILD!"=="ON" if /I not "!START_VS_AFTER_BUILD!"=="OFF" (
    echo.
    echo ERROR: Invalid value for START_VS_AFTER_BUILD. Must be ON or OFF.
    echo Usage: %0 [START_VS_AFTER_BUILD=ON^|OFF]
    goto :end
)

:: ********************************************************************************
:: * This batch file must be run in the same folder as CMakeLists.txt. It is      *
:: * recommended to run it in a Developer PowerShell or Developer Command Prompt  *
:: * for Visual Studio to ensure all necessary Visual Studio files are found.     *
:: *                                                                              *
:: * The script provides a menu to select build options (shared/static library,   *
:: * include noiseutils, include examples). Each option builds both Debug and     *
:: * Release configurations.                                                      *
:: *                                                                              *
:: * By default, Visual Studio will not launch after the build process. To enable *
:: * this, run the script with the START_VS_AFTER_BUILD=ON parameter:             *
:: *     build_vs2022.bat START_VS_AFTER_BUILD=ON                                 *
:: *                                                                              *
:: * If you do not need a symbolic link to the solution file in the source        *
:: * directory, you can safely ignore the warning at the end of this script.      *
:: *                                                                              *
:: * The only modification typically required is to update the solution filename  *
:: * near the top of this file to match the project name defined in your          *
:: * CMakeLists.txt file.                                                         *
:: *                                                                              *
:: * For example, if your CMakeLists.txt contains:                                *
:: *     project(MyProject CXX)                                                   *
:: * then set the solution filename at the top of this batch file to:             *
:: *     @set "SLN_FILE=MyProject.sln"                                            *
:: *                                                                              *
:: * The solution filename will match the project name specified in               *
:: * CMakeLists.txt.                                                              *
:: *                                                                              *
:: * -----------------------------------------------------------------------------*
:: *                                                                              *
:: *       This batch file was updated by Grok for your convenience.              *
:: *               You may distribute, modify, and use it freely.                 *
:: *                                                                              *
:: ********************************************************************************

:: Set the working directory to the location of the batch file
cd /d "%~dp0"

echo.
echo Searching for cmake.exe using the where command...
echo.
where cmake.exe
echo.

:: Use the where command to find cmake.exe
for /f "tokens=*" %%i in ('where cmake.exe 2^>nul') do (
    set "CMAKE_EXE=%%i"
    goto :cmake_found
)

echo.
path
echo.

:cmake_found
if not defined CMAKE_EXE (
    echo.
    echo ERROR: cmake.exe not found in the system PATH.
    echo.
    echo Please ensure Visual Studio 2022 with CMake support is installed and cmake.exe is in the PATH.
    goto :end
)

echo.
echo Found cmake.exe at: %CMAKE_EXE%

:: Check if CMakeLists.txt exists in the current directory
if not exist "CMakeLists.txt" (
    echo.
    echo ERROR: CMakeLists.txt not found in the current directory.
    goto :end
)

:: Set the source directory to the current directory
set "SOURCE_DIR=%CD%"

:: Set the build directory as a subdirectory of the source directory
set "BUILD_DIR=%SOURCE_DIR%\build"

:: Set full path to the solution file created by CMake (in the build folder)
set "CMAKE_SLN_FILE=%BUILD_DIR%\%SLN_FILE%"

:: Set full path to the symbolic link solution file (in the source folder)
set "MLINK_SLN_FILE=%SOURCE_DIR%\%SLN_FILE%"

:: Menu for build options
:menu
cls
echo.
echo.===========================================================
echo        Build Configuration Menu for libnoise-modern
echo  - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
echo   Github: https://github.com/TEK-Nemesis/libnoise-modern
echo ===========================================================
echo.
echo Shared Library (DLL) Builds (Debug and Release):
echo 1. Build Shared Library (DLL), libnoise only
echo 2. Build Shared Library (DLL), libnoise with noiseutils only
echo 3. Build Shared Library (DLL), libnoise with noiseutils and examples
echo.
echo Static Library Builds (Debug and Release):
echo 4. Build Static Library, libnoise only
echo 5. Build Static Library, libnoise with noiseutils only
echo 6. Build Static Library, libnoise with noiseutils and examples
echo.
echo 7. Exit
echo.
set /p CHOICE="Select an option (1-7): "

if "%CHOICE%"=="1" (
    set "SHARED_LIBS=ON"
    set "NOISEUTILS=OFF"
    set "EXAMPLES=OFF"
    goto :build
)
if "%CHOICE%"=="2" (
    set "SHARED_LIBS=ON"
    set "NOISEUTILS=ON"
    set "EXAMPLES=OFF"
    goto :build
)
if "%CHOICE%"=="3" (
    set "SHARED_LIBS=ON"
    set "NOISEUTILS=ON"
    set "EXAMPLES=ON"
    goto :build
)
if "%CHOICE%"=="4" (
    set "SHARED_LIBS=OFF"
    set "NOISEUTILS=OFF"
    set "EXAMPLES=OFF"
    goto :build
)
if "%CHOICE%"=="5" (
    set "SHARED_LIBS=OFF"
    set "NOISEUTILS=ON"
    set "EXAMPLES=OFF"
    goto :build
)
if "%CHOICE%"=="6" (
    set "SHARED_LIBS=OFF"
    set "NOISEUTILS=ON"
    set "EXAMPLES=ON"
    goto :build
)
if "%CHOICE%"=="7" (
    goto :end
)
echo.
echo Invalid option. Please select a number between 1 and 7.
pause
goto :menu

:build
:: Delete old build and Visual Studio files in the root and subdirectories
echo Cleaning old build artifacts...
rmdir /S /Q build 2>nul
rmdir /S /Q .vs 2>nul
rmdir /S /Q out 2>nul
del /S /Q *.sln 2>nul
:: Clean subdirectories
for %%d in (noise examples noiseutils) do (
    if exist "%%d\build" (
        rmdir /S /Q "%%d\build" 2>nul
    )
    if exist "%%d\out" (
        rmdir /S /Q "%%d\out" 2>nul
    )    
    if exist "%%d\.vs" (
        rmdir /S /Q "%%d\.vs" 2>nul
    )
    if exist "%%d" (
        del /S /Q "%%d\*.sln" 2>nul
    )
)

:: Create build directory if it doesn't exist
echo.
echo Creating build directory...
if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
    if !ERRORLEVEL! neq 0 (
        echo.
        echo ERROR: Failed to create build directory: %BUILD_DIR%
        goto :end
    )
)

:: Generate solution in the build directory
echo.
echo Generating solution in build directory...
cmake -S "%SOURCE_DIR%" -B "%BUILD_DIR%" -G "Visual Studio 17 2022" -A x64 -DBUILD_SHARED_LIBS=%SHARED_LIBS% -DBUILD_NOISEUTILS=%NOISEUTILS% -DBUILD_EXAMPLES=%EXAMPLES%
if !ERRORLEVEL! neq 0 (
    echo.
    echo ERROR: CMake generation failed. Please check the output above for details.
    goto :cmake_error
)

:: Build both Debug and Release configurations
echo.
echo Building Debug configuration...
cmake --build "%BUILD_DIR%" --config Debug
if !ERRORLEVEL! neq 0 (
    echo.
    echo ERROR: Debug build failed. Please check the output above for details.
    goto :cmake_error
)

echo.
echo Building Release configuration...
cmake --build "%BUILD_DIR%" --config Release
if !ERRORLEVEL! neq 0 (
    echo.
    echo ERROR: Release build failed. Please check the output above for details.
    goto :cmake_error
)

:: Check if the solution file exists in the build directory
if not exist "%CMAKE_SLN_FILE%" goto :cmake_error

echo.
echo Solution generated successfully in build directory!
echo Solution: %CMAKE_SLN_FILE%

:: Create a symbolic link to the solution file in the source directory
:: (Requires administrator privileges)
echo.
echo Creating symbolic link to solution file in source directory...
:: Remove existing symbolic link if it exists
if exist "%MLINK_SLN_FILE%" (
    del "%MLINK_SLN_FILE%"
)
mklink "%MLINK_SLN_FILE%" "%CMAKE_SLN_FILE%" || goto :mklink_error
if exist "%MLINK_SLN_FILE%" (
    echo.
    echo Solution linked successfully!
    echo Solution: %MLINK_SLN_FILE%
) else (
    goto :mklink_error
)

:: Launch Visual Studio if START_VS_AFTER_BUILD is ON
if /I "!START_VS_AFTER_BUILD!"=="ON" (
    echo.
    echo Launching Visual Studio with the solution in the source directory...
    start "" "%MLINK_SLN_FILE%"
)

goto :end

:cmake_error
:: Cannot continue because CMake failed to create the solution file or build failed
echo.
echo ERROR: The solution file (%SLN_FILE%) was not found in the build directory or the build failed!
echo.
echo Note: You may need to run this batch file in a Developer PowerShell or Developer Command Prompt.
echo       Additionally, administrator privileges are required to create a symbolic link to the solution file.
goto :end

:mklink_error
:: Failed to create symbolic link, but the original solution file exists
echo.
echo WARNING: Failed to create a symbolic link to the solution file in the source directory!
echo.
echo This step is optional but convenient for having a solution file in the source directory.
echo Re-run this batch file with administrator privileges to create the symbolic link.
echo.
if /I "!START_VS_AFTER_BUILD!"=="ON" (
    echo Launching Visual Studio with the solution file in the build directory...
    start "" "%CMAKE_SLN_FILE%"
)

:end
echo.
pause
endlocal