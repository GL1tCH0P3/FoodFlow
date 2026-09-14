@echo off
setlocal EnableExtensions EnableDelayedExpansion

echo.
echo ========================================
echo        FOODFLOW - COMPILACION
echo ========================================
echo.

REM ------------------------------------------------------------
REM Ubicarse en la raiz del proyecto
REM ------------------------------------------------------------

cd /d "%~dp0.."

set "ROOT=%CD%"
set "BUILD_DIR=%ROOT%\build"
set "DIST_DIR=%ROOT%\dist"

echo [1/6] Preparando entorno de compilacion...


REM ------------------------------------------------------------
REM Localizar Visual Studio / Build Tools
REM ------------------------------------------------------------

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if not exist "%VSWHERE%" (
    echo.
    echo ERROR: No se encontro vswhere.exe.
    echo Verifica que Visual Studio Build Tools este instalado.
    exit /b 1
)

set "VS_PATH="

for /f "usebackq tokens=*" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
    set "VS_PATH=%%I"
)

if not defined VS_PATH (
    echo.
    echo ERROR: No se encontro una instalacion de MSVC C++.
    exit /b 1
)

call "%VS_PATH%\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul

where cl.exe >nul 2>&1

if errorlevel 1 (
    echo.
    echo ERROR: cl.exe no esta disponible.
    exit /b 1
)

echo       MSVC encontrado correctamente.


REM ------------------------------------------------------------
REM Verificar dependencias de vcpkg
REM ------------------------------------------------------------

echo [2/6] Verificando libpqxx...

set "VCPKG_DIR=%ROOT%\vcpkg_installed\x64-windows"
set "INCLUDE_DIR=%VCPKG_DIR%\include"
set "LIB_DIR=%VCPKG_DIR%\lib"
set "BIN_DIR=%VCPKG_DIR%\bin"

if not exist "%INCLUDE_DIR%\pqxx\pqxx" (
    echo.
    echo ERROR: libpqxx no esta instalada para FoodFlow.
    echo.
    echo Ejecuta primero:
    echo.
    echo    vcpkg install --triplet x64-windows
    echo.
    exit /b 1
)

if not exist "%LIB_DIR%\libpq.lib" (
    echo.
    echo ERROR: No se encontro libpq.lib.
    exit /b 1
)


REM ------------------------------------------------------------
REM Encontrar biblioteca pqxx
REM ------------------------------------------------------------

set "PQXX_LIB="

for /f "delims=" %%F in ('dir /b /a-d "%LIB_DIR%\*pqxx*.lib" 2^>nul') do (
    if not defined PQXX_LIB (
        set "PQXX_LIB=%LIB_DIR%\%%F"
    )
)

if not defined PQXX_LIB (
    echo.
    echo ERROR: No se encontro la biblioteca de libpqxx.
    exit /b 1
)

echo       libpqxx encontrada.


REM ------------------------------------------------------------
REM Crear directorios
REM ------------------------------------------------------------

echo [3/6] Preparando directorios...

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if not exist "%DIST_DIR%" mkdir "%DIST_DIR%"


REM ------------------------------------------------------------
REM Localizar automaticamente todos los .cpp de src
REM ------------------------------------------------------------

echo [4/6] Buscando archivos fuente...

set "SOURCES="

for /r "%ROOT%\src" %%F in (*.cpp) do (
    set SOURCES=!SOURCES! "%%F"
)

if not defined SOURCES (
    echo.
    echo ERROR: No se encontraron archivos .cpp en src.
    exit /b 1
)


REM ------------------------------------------------------------
REM Compilar
REM ------------------------------------------------------------

echo [5/6] Compilando FoodFlow...
echo.

pushd "%BUILD_DIR%"

cl.exe ^
    /nologo ^
    /std:c++17 ^
    /EHsc ^
    /W4 ^
    /MD ^
    /I"%ROOT%\src" ^
    /I"%INCLUDE_DIR%" ^
    !SOURCES! ^
    /Fe:foodflow.exe ^
    /link ^
    /LIBPATH:"%LIB_DIR%" ^
    "%PQXX_LIB%" ^
    "%LIB_DIR%\libpq.lib"

set "BUILD_RESULT=!ERRORLEVEL!"

popd

if not "%BUILD_RESULT%"=="0" (
    echo.
    echo ========================================
    echo ERROR: La compilacion fallo.
    echo ========================================
    exit /b %BUILD_RESULT%
)


REM ------------------------------------------------------------
REM Preparar distribucion
REM ------------------------------------------------------------

echo.
echo [6/6] Preparando paquete ejecutable...

copy /Y "%BUILD_DIR%\foodflow.exe" "%DIST_DIR%\foodflow.exe" >nul

if exist "%BIN_DIR%\*.dll" (
    copy /Y "%BIN_DIR%\*.dll" "%DIST_DIR%\" >nul
)


echo.
echo ========================================
echo FOODFLOW COMPILADO CORRECTAMENTE
echo ========================================
echo.
echo Ejecutable:
echo %DIST_DIR%\foodflow.exe
echo.

exit /b 0