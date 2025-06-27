@ECHO OFF
REM A simple utility to make, build, copy, and clean a build directory.
REM This script should be run from the root directory of your project (where 'build' will be created).
REM
REM IMPORTANT: To copy files to system directories like C:\Windows\System32,
REM you must run this batch file as an ADMINISTRATOR.

SETLOCAL ENABLEDELAYEDEXPANSION
REM ENABLEDELAYEDEXPANSION é útil para variáveis que mudam dentro de blocos IF/FOR/WHILE.
REM Embora não seja estritamente necessário para este script específico, é uma boa prática para scripts mais complexos.

REM Store the current directory to return to later
SET "curr_dir=%CD%"
SET "build_dir=build"
SET "executable_name=rhythin.exe" REM Assuming .exe for Windows executable
SET "install_path=%SystemRoot%\System32" REM Common system directory for executables

ECHO Current working directory: %curr_dir%

REM Check if the build directory exists
IF NOT EXIST "%build_dir%\" (
    SET /P ans="Build directory not found... Do you want to generate the build directory? (y/Y/s/S/n/N) "
    
    REM Handle multiple affirmative answers (y, Y, s, S)
    IF /I "!ans!"=="y" GOTO :proceed_with_cmake
    IF /I "!ans!"=="s" GOTO :proceed_with_cmake
    
    ECHO Aborting script as build directory generation was declined.
    GOTO :cleanup_and_exit_success
) ELSE (
    ECHO Build directory "%build_dir%" already exists. Proceeding.
)

:proceed_with_cmake
ECHO Attempting to generate build directory with CMake...
REM Use cmake -B to create and configure the build directory. 
REM In -DCMAKE_BUILD_TYPE you can choose between release (Release) and debug (Debug).
REM The following line explicitly sets the generator and compiler paths for MinGW64.
cmake -G "MinGW Makefiles" -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER="C:\mingw64\bin\gcc.exe" -DCMAKE_CXX_COMPILER="C:\mingw64\bin\g++.exe"
IF %ERRORLEVEL% NEQ 0 (
    ECHO Error: Could not generate the build directory with CMake. Exiting.
    GOTO :cleanup_and_exit_error
) ELSE (
    ECHO Build directory generated successfully. Proceeding with build.
)

REM Change to the build directory
REM 2>NUL redirects stderr to NUL (silences error messages from cd)
CD "%build_dir%" 2>NUL
IF %ERRORLEVEL% NEQ 0 (
    ECHO Error: Could not change to "%build_dir%" directory. Aborting.
    GOTO :cleanup_and_exit_error
)

REM Now we are inside the 'build' directory.
ECHO.
ECHO Successfully entered "%build_dir%" directory: %CD%
ECHO.

REM Check if the executable exists
IF EXIST "%executable_name%" (
    ECHO Executable "%executable_name%" found in "%build_dir%".
    ECHO Proceeding with copy and cleanup.
) ELSE (
    ECHO Executable "%executable_name%" not found in "%build_dir%".
    ECHO Attempting to run 'make' to build "%executable_name%"...
    
    REM Run make to build the project
    REM Note: 'make' is generally not native to Windows. You might need 'nmake',
    REM 'mingw32-make', 'msbuild', or ensure 'make.exe' is in your PATH.
    make
    IF %ERRORLEVEL% NEQ 0 (
        ECHO Error: 'make' command failed. Could not build "%executable_name%". Exiting.
        GOTO :cleanup_and_exit_error
    ) ELSE (
        ECHO 'make' completed successfully. Checking for executable again.
        IF NOT EXIST "%executable_name%" (
            ECHO Error: 'make' completed, but "%executable_name%" is still not found.
            GOTO :cleanup_and_exit_error
        )
    )
)

REM If we reach here, the executable should exist and be ready for copy.
ECHO Attempting to copy "%executable_name%" to "%install_path%"...
REM Copy the executable to the system path
COPY "%executable_name%" "%install_path%\" >NUL
IF %ERRORLEVEL% NEQ 0 (
    ECHO Error: Failed to copy "%executable_name%" to "%install_path%". Check permissions (run as Administrator?).
    GOTO :cleanup_and_exit_error
) ELSE (
    ECHO "%executable_name%" copied to "%install_path%" successfully.
    ECHO Initiating cleanup in "%build_dir%"...

    REM Remove the executable from the build directory (it's a file)
    DEL "%executable_name%" >NUL
    IF %ERRORLEVEL% NEQ 0 (
        ECHO Error: Failed to remove "%executable_name%" from "%build_dir%".
        GOTO :cleanup_and_exit_error
    ) ELSE (
        ECHO Cleaned "%executable_name%" from "%build_dir%" successfully.
        ECHO Script finished successfully.
        GOTO :cleanup_and_exit_success
    )
)


:cleanup_and_exit_success
REM Return to the original directory before exiting
CD "%curr_dir%" 2>NUL
IF %ERRORLEVEL% NEQ 0 (
    ECHO Error: Could not return to original directory "%curr_dir%".
)
ENDLOCAL
EXIT /B 0

:cleanup_and_exit_error
REM Return to the original directory before exiting
CD "%curr_dir%" 2>NUL
IF %ERRORLEVEL% NEQ 0 (
    ECHO Error: Could not return to original directory "%curr_dir%".
)
ENDLOCAL
EXIT /B 1
