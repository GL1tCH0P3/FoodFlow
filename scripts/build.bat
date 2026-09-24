@echo off
setlocal EnableExtensions EnableDelayedExpansion

REM ============================================================
REM FOODFLOW - BUILD
REM
REM Responsabilidad:
REM   - Preparar MSVC x64.
REM   - Verificar dependencias precompiladas.
REM   - Localizar todos los .cpp dentro de src.
REM   - Compilar y enlazar FoodFlow.
REM   - Generar dist\foodflow.exe.
REM   - Copiar DLL de runtime.
REM
REM NO instala dependencias.
REM NO usa vcpkg.
REM NO descarga archivos.
REM ============================================================

cd /d "%~dp0.."

set "ROOT=%CD%"

set "SRC_DIR=!ROOT!\src"
set "BUILD_DIR=!ROOT!\build"
set "DIST_DIR=!ROOT!\dist"

set "DEPS_DIR=!ROOT!\.deps\x64-windows"

set "INCLUDE_DIR=!DEPS_DIR!\include"
set "LIB_DIR=!DEPS_DIR!\lib"
set "BIN_DIR=!DEPS_DIR!\bin"


echo.
echo ========================================
echo        FOODFLOW - COMPILACION
echo ========================================
echo.


REM ============================================================
REM 1. PREPARAR MSVC
REM ============================================================

echo [1/6] Preparando compilador C++...

call "!ROOT!\scripts\msvc_env.bat"

if errorlevel 1 goto ERROR_MSVC

echo       Visual Studio:
echo       !FOODFLOW_VS_PATH!
echo.
echo       Toolset MSVC:
echo       !FOODFLOW_MSVC_VERSION!
echo.
echo       Arquitectura: x64
echo       MSVC preparado correctamente.


REM ============================================================
REM 2. VERIFICAR DEPENDENCIAS
REM ============================================================

echo.
echo [2/6] Verificando dependencias...

if not exist "!DEPS_DIR!\" goto ERROR_DEPS

if not exist "!INCLUDE_DIR!\pqxx\" goto ERROR_PQXX_HEADERS

if not exist "!LIB_DIR!\libpq.lib" goto ERROR_LIBPQ


REM ------------------------------------------------------------
REM Localizar biblioteca libpqxx
REM ------------------------------------------------------------

if not exist "!BUILD_DIR!\" mkdir "!BUILD_DIR!"

set "PQXX_LIB_NAME="
set "PQXX_LIB_FILE=!BUILD_DIR!\pqxx_lib.txt"

dir /b /a-d "!LIB_DIR!\*pqxx*.lib" > "!PQXX_LIB_FILE!" 2>nul

if exist "!PQXX_LIB_FILE!" set /p PQXX_LIB_NAME=<"!PQXX_LIB_FILE!"

del /Q "!PQXX_LIB_FILE!" >nul 2>&1

if not defined PQXX_LIB_NAME goto ERROR_PQXX_LIB

set "PQXX_LIB=!LIB_DIR!\!PQXX_LIB_NAME!"

if not exist "!PQXX_LIB!" goto ERROR_PQXX_LIB

echo       libpqxx encontrada:
echo       !PQXX_LIB_NAME!
echo.
echo       libpq encontrada:
echo       libpq.lib
echo.
echo       Dependencias correctas.


REM ============================================================
REM 3. PREPARAR DIRECTORIOS
REM ============================================================

echo.
echo [3/6] Preparando directorios...

if not exist "!BUILD_DIR!\" mkdir "!BUILD_DIR!"

if not exist "!DIST_DIR!\" mkdir "!DIST_DIR!"

echo       Directorios preparados.


REM ============================================================
REM 4. LOCALIZAR ARCHIVOS FUENTE
REM ============================================================

echo.
echo [4/6] Buscando archivos fuente...

if not exist "!SRC_DIR!\" goto ERROR_SRC_DIR

set "SOURCES_RSP=!BUILD_DIR!\sources.rsp"

if exist "!SOURCES_RSP!" del /Q "!SOURCES_RSP!"


REM ------------------------------------------------------------
REM PowerShell se encarga de recorrer src recursivamente.
REM
REM Se utilizan variables de entorno para evitar problemas
REM de quoting con rutas que contienen espacios.
REM ------------------------------------------------------------

set "FOODFLOW_SRC_DIR=!SRC_DIR!"
set "FOODFLOW_SOURCES_RSP=!SOURCES_RSP!"

powershell.exe ^
    -NoProfile ^
    -ExecutionPolicy Bypass ^
    -Command ^
    "$files = @(Get-ChildItem -LiteralPath $env:FOODFLOW_SRC_DIR -Recurse -File -Filter '*.cpp' -ErrorAction Stop); if ($files.Count -eq 0) { exit 2 }; $lines = $files | ForEach-Object { '\"' + $_.FullName + '\"' }; Set-Content -LiteralPath $env:FOODFLOW_SOURCES_RSP -Value $lines -Encoding ASCII; Write-Host ('      ' + $files.Count + ' archivo(s) .cpp encontrado(s).')" 

if errorlevel 1 goto ERROR_SOURCES

if not exist "!SOURCES_RSP!" goto ERROR_SOURCES

for %%A in ("!SOURCES_RSP!") do set "SOURCES_SIZE=%%~zA"

if "!SOURCES_SIZE!"=="0" goto ERROR_SOURCES

echo       Lista de fuentes preparada correctamente.


REM ============================================================
REM 5. COMPILAR Y ENLAZAR
REM ============================================================

echo.
echo [5/6] Compilando FoodFlow...
echo.

pushd "!BUILD_DIR!"

"!FOODFLOW_CL!" ^
    /nologo ^
    /std:c++17 ^
    /EHsc ^
    /W4 ^
    /MD ^
    /utf-8 ^
    /I"!SRC_DIR!" ^
    /I"!INCLUDE_DIR!" ^
    @"!SOURCES_RSP!" ^
    /Fe:"foodflow.exe" ^
    /link ^
    /LIBPATH:"!LIB_DIR!" ^
    "!PQXX_LIB!" ^
    "!LIB_DIR!\libpq.lib"

set "BUILD_RESULT=!ERRORLEVEL!"

popd

if not "!BUILD_RESULT!"=="0" goto ERROR_BUILD


REM ============================================================
REM 6. PREPARAR DISTRIBUCION
REM ============================================================

echo.
echo [6/6] Preparando distribucion...

if exist "!DIST_DIR!\foodflow.exe" del /Q "!DIST_DIR!\foodflow.exe"

del /Q "!DIST_DIR!\*.dll" >nul 2>&1


REM ------------------------------------------------------------
REM Copiar ejecutable
REM ------------------------------------------------------------

copy /Y ^
    "!BUILD_DIR!\foodflow.exe" ^
    "!DIST_DIR!\foodflow.exe" ^
    >nul

if errorlevel 1 goto ERROR_COPY_EXE


REM ------------------------------------------------------------
REM Copiar DLL de runtime
REM ------------------------------------------------------------

if not exist "!BIN_DIR!\" goto RUNTIME_READY

copy /Y ^
    "!BIN_DIR!\*.dll" ^
    "!DIST_DIR!\" ^
    >nul 2>&1


:RUNTIME_READY

echo.
echo ========================================
echo FOODFLOW COMPILADO CORRECTAMENTE
echo ========================================
echo.
echo Toolset:
echo   MSVC !FOODFLOW_MSVC_VERSION!
echo.
echo Arquitectura:
echo   x64
echo.
echo Ejecutable:
echo   !DIST_DIR!\foodflow.exe
echo.

exit /b 0


REM ============================================================
REM ERRORES
REM ============================================================

:ERROR_MSVC

echo.
echo ========================================
echo ERROR: No fue posible preparar MSVC
echo ========================================
echo.
exit /b 1


:ERROR_DEPS

echo.
echo ========================================
echo ERROR: Dependencias no preparadas
echo ========================================
echo.
echo No existe:
echo.
echo !DEPS_DIR!
echo.
echo Ejecute:
echo.
echo   scripts\setup.bat
echo.
exit /b 1


:ERROR_PQXX_HEADERS

echo.
echo ========================================
echo ERROR: Headers libpqxx no encontrados
echo ========================================
echo.
echo Ruta esperada:
echo.
echo !INCLUDE_DIR!\pqxx
echo.
exit /b 1


:ERROR_LIBPQ

echo.
echo ========================================
echo ERROR: libpq.lib no encontrada
echo ========================================
echo.
echo Ruta esperada:
echo.
echo !LIB_DIR!\libpq.lib
echo.
exit /b 1


:ERROR_PQXX_LIB

echo.
echo ========================================
echo ERROR: Biblioteca libpqxx no encontrada
echo ========================================
echo.
echo Directorio:
echo.
echo !LIB_DIR!
echo.
exit /b 1


:ERROR_SRC_DIR

echo.
echo ========================================
echo ERROR: Directorio src no encontrado
echo ========================================
echo.
echo Ruta esperada:
echo.
echo !SRC_DIR!
echo.
exit /b 1


:ERROR_SOURCES

echo.
echo ========================================
echo ERROR: No se encontraron archivos .cpp
echo ========================================
echo.
echo Directorio revisado:
echo.
echo !SRC_DIR!
echo.
echo Verifique manualmente con:
echo.
echo   Get-ChildItem ".\src" -Recurse -File -Filter *.cpp
echo.
exit /b 1


:ERROR_BUILD

echo.
echo ========================================
echo ERROR: LA COMPILACION FALLO
echo ========================================
echo.
exit /b !BUILD_RESULT!


:ERROR_COPY_EXE

echo.
echo ========================================
echo ERROR: No fue posible preparar dist
echo ========================================
echo.
echo No se pudo copiar foodflow.exe.
echo.
exit /b 1