@echo off
setlocal EnableExtensions EnableDelayedExpansion

REM ============================================================
REM FOODFLOW - BUILD
REM
REM Compila:
REM   - Codigo FoodFlow
REM   - Dear ImGui
REM   - Backend Win32
REM   - Backend DirectX 11
REM
REM Enlaza:
REM   - libpqxx
REM   - libpq
REM   - DirectX 11
REM
REM Genera:
REM   dist\foodflow.exe
REM   dist\*.dll
REM
REM NO instala dependencias.
REM NO usa vcpkg.
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

set "IMGUI_DIR=!ROOT!\vendor\imgui"
set "IMGUI_BACKENDS=!IMGUI_DIR!\backends"


echo.
echo ========================================
echo        FOODFLOW - COMPILACION
echo ========================================
echo.


REM ============================================================
REM 1. PREPARAR MSVC
REM ============================================================

echo [1/7] Preparando compilador C++...

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
REM 2. VERIFICAR DEPENDENCIAS POSTGRESQL
REM ============================================================

echo.
echo [2/7] Verificando dependencias PostgreSQL...


if not exist "!DEPS_DIR!\" goto ERROR_DEPS

if not exist "!INCLUDE_DIR!\pqxx\" goto ERROR_PQXX_HEADERS

if not exist "!LIB_DIR!\libpq.lib" goto ERROR_LIBPQ


REM ------------------------------------------------------------
REM Buscar biblioteca libpqxx.
REM
REM Primero intentamos los nombres habituales.
REM Si el paquete utiliza otro nombre compatible, usamos fallback.
REM ------------------------------------------------------------

set "PQXX_LIB_NAME="


if exist "!LIB_DIR!\libpqxx.lib" (
    set "PQXX_LIB_NAME=libpqxx.lib"
)


if not defined PQXX_LIB_NAME (
    if exist "!LIB_DIR!\pqxx.lib" (
        set "PQXX_LIB_NAME=pqxx.lib"
    )
)


if not defined PQXX_LIB_NAME (

    set "PQXX_LIB_FILE=!TEMP!\foodflow_pqxx_lib_!RANDOM!_!RANDOM!.txt"


    dir /b /a-d ^
        "!LIB_DIR!\*pqxx*.lib" ^
        > "!PQXX_LIB_FILE!" ^
        2>nul


    if exist "!PQXX_LIB_FILE!" (
        set /p PQXX_LIB_NAME=<"!PQXX_LIB_FILE!"
    )


    del /Q "!PQXX_LIB_FILE!" >nul 2>&1
)


if not defined PQXX_LIB_NAME goto ERROR_PQXX_LIB


set "PQXX_LIB=!LIB_DIR!\!PQXX_LIB_NAME!"


echo       libpqxx: !PQXX_LIB_NAME!
echo       libpq:   libpq.lib
echo       Dependencias PostgreSQL correctas.


REM ============================================================
REM 3. VERIFICAR DEAR IMGUI
REM ============================================================

echo.
echo [3/7] Verificando Dear ImGui...


if not exist "!IMGUI_DIR!\imgui.h" goto ERROR_IMGUI

if not exist "!IMGUI_DIR!\imgui.cpp" goto ERROR_IMGUI

if not exist "!IMGUI_DIR!\imgui_draw.cpp" goto ERROR_IMGUI

if not exist "!IMGUI_DIR!\imgui_tables.cpp" goto ERROR_IMGUI

if not exist "!IMGUI_DIR!\imgui_widgets.cpp" goto ERROR_IMGUI


if not exist "!IMGUI_BACKENDS!\imgui_impl_win32.cpp" goto ERROR_IMGUI

if not exist "!IMGUI_BACKENDS!\imgui_impl_win32.h" goto ERROR_IMGUI


if not exist "!IMGUI_BACKENDS!\imgui_impl_dx11.cpp" goto ERROR_IMGUI

if not exist "!IMGUI_BACKENDS!\imgui_impl_dx11.h" goto ERROR_IMGUI


echo       Dear ImGui encontrado.
echo       Backend Win32 encontrado.
echo       Backend DirectX 11 encontrado.


REM ============================================================
REM 4. PREPARAR BUILD
REM
REM Se limpia build para evitar objetos antiguos.
REM dist NO se elimina hasta que la compilacion termine bien.
REM ============================================================

echo.
echo [4/7] Preparando directorio de compilacion...


if exist "!BUILD_DIR!\" (
    rmdir /S /Q "!BUILD_DIR!"
)


if exist "!BUILD_DIR!\" goto ERROR_CLEAN_BUILD


mkdir "!BUILD_DIR!"


if errorlevel 1 goto ERROR_BUILD_DIR


echo       Build limpio preparado.


REM ============================================================
REM 5. PREPARAR FUENTES
REM ============================================================

echo.
echo [5/7] Buscando archivos fuente...


if not exist "!SRC_DIR!\" goto ERROR_SRC_DIR


set "SOURCES_RSP=!BUILD_DIR!\sources.rsp"


REM ------------------------------------------------------------
REM Fuentes propias de FoodFlow
REM ------------------------------------------------------------

set "FOODFLOW_SRC_DIR=!SRC_DIR!"

set "FOODFLOW_SOURCES_RSP=!SOURCES_RSP!"


powershell.exe ^
    -NoProfile ^
    -ExecutionPolicy Bypass ^
    -Command ^
    "$files = @(Get-ChildItem -LiteralPath $env:FOODFLOW_SRC_DIR -Recurse -File -Filter '*.cpp' -ErrorAction Stop); if ($files.Count -eq 0) { exit 2 }; $files | ForEach-Object { '\"' + $_.FullName + '\"' } | Set-Content -LiteralPath $env:FOODFLOW_SOURCES_RSP -Encoding ASCII; Write-Host ('      ' + $files.Count + ' archivo(s) FoodFlow encontrado(s).')"


if errorlevel 1 goto ERROR_SOURCES


REM ------------------------------------------------------------
REM Fuentes Dear ImGui
REM ------------------------------------------------------------

>>"!SOURCES_RSP!" echo "!IMGUI_DIR!\imgui.cpp"

>>"!SOURCES_RSP!" echo "!IMGUI_DIR!\imgui_draw.cpp"

>>"!SOURCES_RSP!" echo "!IMGUI_DIR!\imgui_tables.cpp"

>>"!SOURCES_RSP!" echo "!IMGUI_DIR!\imgui_widgets.cpp"

>>"!SOURCES_RSP!" echo "!IMGUI_BACKENDS!\imgui_impl_win32.cpp"

>>"!SOURCES_RSP!" echo "!IMGUI_BACKENDS!\imgui_impl_dx11.cpp"


echo       6 archivo(s) Dear ImGui agregados.
echo       Lista de fuentes preparada correctamente.


REM ============================================================
REM 6. COMPILAR Y ENLAZAR
REM ============================================================

echo.
echo [6/7] Compilando FoodFlow GUI...
echo.


pushd "!BUILD_DIR!"


"!FOODFLOW_CL!" ^
    /nologo ^
    /std:c++17 ^
    /EHsc ^
    /W4 ^
    /MD ^
    /utf-8 ^
    /DUNICODE ^
    /D_UNICODE ^
    /I"!SRC_DIR!" ^
    /I"!INCLUDE_DIR!" ^
    /I"!IMGUI_DIR!" ^
    /I"!IMGUI_BACKENDS!" ^
    @"!SOURCES_RSP!" ^
    /Fe:"foodflow.exe" ^
    /link ^
    /LIBPATH:"!LIB_DIR!" ^
    "!PQXX_LIB!" ^
    "!LIB_DIR!\libpq.lib" ^
    d3d11.lib ^
    dxgi.lib ^
    d3dcompiler.lib


set "BUILD_RESULT=!ERRORLEVEL!"


popd


if not "!BUILD_RESULT!"=="0" goto ERROR_BUILD


if not exist "!BUILD_DIR!\foodflow.exe" goto ERROR_BUILD_EXE


REM ============================================================
REM 7. PREPARAR DISTRIBUCION
REM
REM dist se reconstruye completamente para garantizar que
REM foodflow.exe y sus DLL pertenezcan al mismo build.
REM ============================================================

echo.
echo [7/7] Preparando distribucion...


if exist "!DIST_DIR!\" (
    rmdir /S /Q "!DIST_DIR!"
)


if exist "!DIST_DIR!\" goto ERROR_CLEAN_DIST


mkdir "!DIST_DIR!"


if errorlevel 1 goto ERROR_DIST_DIR


REM ------------------------------------------------------------
REM Ejecutable
REM ------------------------------------------------------------

copy /Y ^
    "!BUILD_DIR!\foodflow.exe" ^
    "!DIST_DIR!\foodflow.exe" ^
    >nul


if errorlevel 1 goto ERROR_COPY_EXE


REM ------------------------------------------------------------
REM DLL runtime PostgreSQL / libpqxx y dependencias asociadas.
REM
REM Copiamos TODAS las DLL de x64-windows\bin.
REM De esta forma no dependemos del PATH de la maquina.
REM ------------------------------------------------------------

set /A RUNTIME_DLL_COUNT=0


if exist "!BIN_DIR!\" (

    for %%F in ("!BIN_DIR!\*.dll") do (

        if exist "%%~fF" (

            copy /Y ^
                "%%~fF" ^
                "!DIST_DIR!\" ^
                >nul


            if errorlevel 1 goto ERROR_COPY_DLL


            set /A RUNTIME_DLL_COUNT+=1
        )
    )
)


echo       Ejecutable preparado.

echo       DLL runtime copiadas: !RUNTIME_DLL_COUNT!


if exist "!BIN_DIR!\libpq.dll" (

    if not exist "!DIST_DIR!\libpq.dll" goto ERROR_COPY_DLL
)


echo.
echo ========================================
echo FOODFLOW GUI COMPILADO CORRECTAMENTE
echo ========================================
echo.
echo Toolset:
echo   MSVC !FOODFLOW_MSVC_VERSION!
echo.
echo Arquitectura:
echo   x64
echo.
echo Estandar:
echo   C++17
echo.
echo GUI:
echo   Dear ImGui + Win32 + DirectX 11
echo.
echo Ejecutable:
echo   !DIST_DIR!\foodflow.exe
echo.
echo Runtime:
echo   !RUNTIME_DLL_COUNT! DLL(s) copiadas a dist
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
echo Ejecute primero:
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
exit /b 1


:ERROR_LIBPQ

echo.
echo ========================================
echo ERROR: libpq.lib no encontrada
echo ========================================
echo.
exit /b 1


:ERROR_PQXX_LIB

echo.
echo ========================================
echo ERROR: Biblioteca libpqxx no encontrada
echo ========================================
echo.
exit /b 1


:ERROR_IMGUI

echo.
echo ========================================
echo ERROR: Dear ImGui incompleto
echo ========================================
echo.
echo Directorio esperado:
echo.
echo   !IMGUI_DIR!
echo.
exit /b 1


:ERROR_CLEAN_BUILD

echo.
echo ========================================
echo ERROR: No fue posible limpiar build
echo ========================================
echo.
echo Verifique que ningun proceso este usando
echo archivos dentro de:
echo.
echo   !BUILD_DIR!
echo.
exit /b 1


:ERROR_BUILD_DIR

echo.
echo ========================================
echo ERROR: No fue posible crear build
echo ========================================
echo.
exit /b 1


:ERROR_SRC_DIR

echo.
echo ========================================
echo ERROR: Directorio src no encontrado
echo ========================================
echo.
exit /b 1


:ERROR_SOURCES

echo.
echo ========================================
echo ERROR: No se encontraron fuentes FoodFlow
echo ========================================
echo.
exit /b 1


:ERROR_BUILD

echo.
echo ========================================
echo ERROR: LA COMPILACION FALLO
echo ========================================
echo.
exit /b !BUILD_RESULT!


:ERROR_BUILD_EXE

echo.
echo ========================================
echo ERROR: Ejecutable no generado
echo ========================================
echo.
echo No se encontro:
echo.
echo   !BUILD_DIR!\foodflow.exe
echo.
exit /b 1


:ERROR_CLEAN_DIST

echo.
echo ========================================
echo ERROR: No fue posible limpiar dist
echo ========================================
echo.
echo Cierre FoodFlow si se encuentra abierto
echo y vuelva a compilar.
echo.
exit /b 1


:ERROR_DIST_DIR

echo.
echo ========================================
echo ERROR: No fue posible crear dist
echo ========================================
echo.
exit /b 1


:ERROR_COPY_EXE

echo.
echo ========================================
echo ERROR: No fue posible copiar foodflow.exe
echo ========================================
echo.
exit /b 1


:ERROR_COPY_DLL

echo.
echo ========================================
echo ERROR: No fue posible preparar las DLL
echo ========================================
echo.
exit /b 1