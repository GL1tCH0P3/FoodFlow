@echo off
setlocal EnableExtensions

cd /d "%~dp0.."

set "ROOT=%CD%"

echo.
echo ========================================
echo          FOODFLOW - SETUP
echo ========================================
echo.

REM ------------------------------------------------------------
REM 1. Localizar vcpkg
REM ------------------------------------------------------------

echo [1/4] Buscando vcpkg...

if defined VCPKG_ROOT (
    if exist "%VCPKG_ROOT%\vcpkg.exe" (
        goto VCPKG_FOUND
    )
)

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if exist "%VSWHERE%" (

    set "VS_PATH="

    for /f "usebackq tokens=*" %%I in (`"%VSWHERE%" -latest -products * -property installationPath`) do (
        set "VS_PATH=%%I"
    )

    if defined VS_PATH (
        if exist "%VS_PATH%\VC\vcpkg\vcpkg.exe" (
            set "VCPKG_ROOT=%VS_PATH%\VC\vcpkg"
            goto VCPKG_FOUND
        )
    )
)

echo.
echo ERROR: No se encontro vcpkg.
echo Instale Visual Studio Build Tools con C++ y vcpkg.
echo.
pause
exit /b 1


:VCPKG_FOUND

echo       vcpkg encontrado:
echo       %VCPKG_ROOT%


REM ------------------------------------------------------------
REM 2. Restaurar dependencias
REM ------------------------------------------------------------

echo.
echo [2/4] Restaurando dependencias...

"%VCPKG_ROOT%\vcpkg.exe" install --triplet x64-windows

if errorlevel 1 (
    echo.
    echo ERROR: No fue posible instalar las dependencias.
    echo.
    pause
    exit /b 1
)


REM ------------------------------------------------------------
REM 3. Crear configuracion local
REM ------------------------------------------------------------

echo.
echo [3/4] Preparando configuracion...

if not exist "%ROOT%\config\database.env" (

    copy ^
        "%ROOT%\config\database.env.example" ^
        "%ROOT%\config\database.env" >nul

    echo.
    echo Se creo:
    echo config\database.env
    echo.
    echo IMPORTANTE:
    echo Complete ese archivo con las credenciales de FoodFlow.
)

REM ------------------------------------------------------------
REM 4. Compilar
REM ------------------------------------------------------------

echo.
echo [4/4] Compilando FoodFlow...
echo.

call "%ROOT%\scripts\build.bat"

if errorlevel 1 (
    echo.
    echo ERROR: La compilacion fallo.
    pause
    exit /b 1
)

echo.
echo ========================================
echo       SETUP COMPLETADO
echo ========================================
echo.
echo Siguiente paso:
echo.
echo   1. Complete config\database.env
echo   2. Ejecute run.bat
echo.
pause