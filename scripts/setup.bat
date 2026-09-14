@echo off
setlocal EnableExtensions EnableDelayedExpansion

cd /d "%~dp0.."

set "ROOT=%CD%"
set "TOOLS_DIR=%ROOT%\.tools"
set "LOCAL_VCPKG=%TOOLS_DIR%\vcpkg"

echo.
echo ========================================
echo          FOODFLOW - SETUP
echo ========================================
echo.

REM ============================================================
REM 1. Verificar Git
REM ============================================================

echo [1/6] Verificando Git...

where git.exe >nul 2>&1

if errorlevel 1 (
    echo.
    echo ERROR: Git no esta instalado o no esta disponible en PATH.
    echo Instale Git y vuelva a ejecutar este archivo.
    echo.
    pause
    exit /b 1
)

echo       Git encontrado.


REM ============================================================
REM 2. Buscar vcpkg existente
REM ============================================================

echo.
echo [2/6] Buscando vcpkg...

set "VCPKG_EXE="

REM Primero: variable de entorno existente
if defined VCPKG_ROOT (
    if exist "%VCPKG_ROOT%\vcpkg.exe" (
        set "VCPKG_EXE=%VCPKG_ROOT%\vcpkg.exe"
        goto VCPKG_READY
    )
)

REM Segundo: vcpkg incluido con Visual Studio / Build Tools
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if exist "%VSWHERE%" (

    set "VS_PATH="

    for /f "usebackq tokens=*" %%I in (`"%VSWHERE%" -latest -products * -property installationPath`) do (
        set "VS_PATH=%%I"
    )

    if defined VS_PATH (
        if exist "!VS_PATH!\VC\vcpkg\vcpkg.exe" (
            set "VCPKG_ROOT=!VS_PATH!\VC\vcpkg"
            set "VCPKG_EXE=!VCPKG_ROOT!\vcpkg.exe"
            goto VCPKG_READY
        )
    )
)

REM Tercero: vcpkg local del proyecto
if exist "%LOCAL_VCPKG%\vcpkg.exe" (
    set "VCPKG_ROOT=%LOCAL_VCPKG%"
    set "VCPKG_EXE=%LOCAL_VCPKG%\vcpkg.exe"
    goto VCPKG_READY
)


REM ============================================================
REM 3. Descargar vcpkg si no existe
REM ============================================================

echo       vcpkg no encontrado.
echo       Instalando copia local para FoodFlow...

if not exist "%TOOLS_DIR%" (
    mkdir "%TOOLS_DIR%"
)

git clone https://github.com/microsoft/vcpkg.git "%LOCAL_VCPKG%"

if errorlevel 1 (
    echo.
    echo ERROR: No fue posible descargar vcpkg.
    pause
    exit /b 1
)

call "%LOCAL_VCPKG%\bootstrap-vcpkg.bat" -disableMetrics

if errorlevel 1 (
    echo.
    echo ERROR: No fue posible inicializar vcpkg.
    pause
    exit /b 1
)

set "VCPKG_ROOT=%LOCAL_VCPKG%"
set "VCPKG_EXE=%LOCAL_VCPKG%\vcpkg.exe"


:VCPKG_READY

echo       vcpkg listo:
echo       %VCPKG_EXE%


REM ============================================================
REM 4. Restaurar dependencias del proyecto
REM ============================================================

echo.
echo [3/6] Restaurando dependencias...

"%VCPKG_EXE%" install --triplet x64-windows

if errorlevel 1 (
    echo.
    echo ERROR: No fue posible restaurar las dependencias.
    pause
    exit /b 1
)

echo       Dependencias restauradas.


REM ============================================================
REM 5. Crear configuracion local
REM ============================================================

echo.
echo [4/6] Preparando configuracion...

if not exist "%ROOT%\config\database.env" (

    if not exist "%ROOT%\config\database.env.example" (
        echo.
        echo ERROR: No existe config\database.env.example
        pause
        exit /b 1
    )

    copy /Y ^
        "%ROOT%\config\database.env.example" ^
        "%ROOT%\config\database.env" >nul

    echo       Se creo config\database.env
    echo       Debe completar las credenciales de PostgreSQL.
) else (
    echo       database.env ya existe.
)


REM ============================================================
REM 6. Compilar
REM ============================================================

echo.
echo [5/6] Compilando FoodFlow...

call "%ROOT%\scripts\build.bat"

if errorlevel 1 (
    echo.
    echo ERROR: La compilacion fallo.
    pause
    exit /b 1
)


REM ============================================================
REM Resultado
REM ============================================================

echo.
echo [6/6] Setup finalizado.

echo.
echo ========================================
echo       FOODFLOW LISTO
echo ========================================
echo.
echo Para continuar:
echo.
echo   1. Abra config\database.env
echo   2. Complete las credenciales entregadas
echo   3. Ejecute run.bat
echo.
pause

exit /b 0