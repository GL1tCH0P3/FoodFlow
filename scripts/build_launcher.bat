@echo off
setlocal EnableExtensions EnableDelayedExpansion

REM ============================================================
REM FOODFLOW - BUILD LAUNCHER
REM
REM Genera:
REM
REM   FoodFlow.exe
REM
REM en la raiz del proyecto.
REM
REM El launcher:
REM   - carga config\database.env
REM   - inicia dist\foodflow.exe
REM   - no muestra consola
REM   - contiene el icono de FoodFlow
REM   - contiene metadatos de version
REM
REM No introduce dependencias adicionales.
REM Utiliza el mismo MSVC del proyecto.
REM ============================================================


cd /d "%~dp0.."


set "ROOT=%CD%"

set "LAUNCHER_DIR=!ROOT!\launcher"

set "LAUNCHER_SRC=!LAUNCHER_DIR!\FoodFlowLauncher.cpp"


set "RESOURCE_DIR=!ROOT!\resources"

set "RESOURCE_RC=!RESOURCE_DIR!\foodflow_launcher.rc"

set "RESOURCE_ICON=!RESOURCE_DIR!\foodflow.ico"


set "BUILD_DIR=!ROOT!\build\launcher"

set "RESOURCE_RES=!BUILD_DIR!\foodflow_launcher.res"


set "OUTPUT_EXE=!ROOT!\FoodFlow.exe"


echo.
echo ========================================
echo       FOODFLOW - BUILD LAUNCHER
echo ========================================
echo.


REM ============================================================
REM 1. PREPARAR MSVC
REM ============================================================

echo [1/4] Preparando compilador...


call "!ROOT!\scripts\msvc_env.bat"


if errorlevel 1 goto ERROR_MSVC


echo       MSVC !FOODFLOW_MSVC_VERSION!
echo       Arquitectura x64


REM ============================================================
REM 2. VERIFICAR ARCHIVOS
REM ============================================================

echo.
echo [2/4] Verificando launcher...


if not exist "!LAUNCHER_SRC!" goto ERROR_SOURCE


if not exist "!RESOURCE_RC!" goto ERROR_RESOURCE


if not exist "!RESOURCE_ICON!" goto ERROR_ICON


echo       Fuente encontrada.
echo       Recursos encontrados.
echo       Icono encontrado.


REM ============================================================
REM 3. PREPARAR BUILD
REM ============================================================

if not exist "!BUILD_DIR!\" (

    mkdir "!BUILD_DIR!"
)


if errorlevel 1 goto ERROR_BUILD_DIR


if exist "!RESOURCE_RES!" (

    del /Q "!RESOURCE_RES!"
)


REM ============================================================
REM 4. COMPILAR RECURSOS WINDOWS
REM
REM Ejecutamos rc.exe desde resources.
REM
REM De esta manera:
REM
REM   ICON "foodflow.ico"
REM
REM siempre se resuelve respecto a:
REM
REM   resources\
REM ============================================================

echo.
echo [3/4] Compilando recursos...


pushd "!RESOURCE_DIR!"


rc.exe ^
    /nologo ^
    /fo"!RESOURCE_RES!" ^
    "foodflow_launcher.rc"


set "RC_RESULT=!ERRORLEVEL!"


popd


if not "!RC_RESULT!"=="0" goto ERROR_RC


if not exist "!RESOURCE_RES!" goto ERROR_RC


echo       Icono compilado.
echo       Metadatos compilados.


REM ============================================================
REM 5. COMPILAR LAUNCHER
REM ============================================================

echo.
echo [4/4] Compilando FoodFlow.exe...
echo.


if exist "!OUTPUT_EXE!" (

    del /Q "!OUTPUT_EXE!"
)


"!FOODFLOW_CL!" ^
    /nologo ^
    /std:c++17 ^
    /EHsc ^
    /W4 ^
    /O2 ^
    /MT ^
    /utf-8 ^
    /DUNICODE ^
    /D_UNICODE ^
    "!LAUNCHER_SRC!" ^
    "!RESOURCE_RES!" ^
    /Fe:"!OUTPUT_EXE!" ^
    /link ^
    /SUBSYSTEM:WINDOWS ^
    user32.lib


set "BUILD_RESULT=!ERRORLEVEL!"


if not "!BUILD_RESULT!"=="0" goto ERROR_BUILD


if not exist "!OUTPUT_EXE!" goto ERROR_EXE


echo.
echo ========================================
echo    FOODFLOW.EXE GENERADO CORRECTAMENTE
echo ========================================
echo.
echo Ejecutable principal:
echo.
echo   !OUTPUT_EXE!
echo.
echo El profesor puede iniciar FoodFlow
echo haciendo doble clic directamente sobre:
echo.
echo   FoodFlow.exe
echo.
echo No se requiere abrir run.bat.
echo.

exit /b 0


REM ============================================================
REM ERRORES
REM ============================================================

:ERROR_MSVC

echo.
echo ========================================
echo ERROR: MSVC no disponible
echo ========================================
echo.
exit /b 1


:ERROR_SOURCE

echo.
echo ========================================
echo ERROR: Launcher no encontrado
echo ========================================
echo.
echo Falta:
echo.
echo   launcher\FoodFlowLauncher.cpp
echo.
exit /b 1


:ERROR_RESOURCE

echo.
echo ========================================
echo ERROR: Recurso no encontrado
echo ========================================
echo.
echo Falta:
echo.
echo   resources\foodflow_launcher.rc
echo.
exit /b 1


:ERROR_ICON

echo.
echo ========================================
echo ERROR: Icono FoodFlow no encontrado
echo ========================================
echo.
echo Falta:
echo.
echo   resources\foodflow.ico
echo.
exit /b 1


:ERROR_BUILD_DIR

echo.
echo ========================================
echo ERROR: No se pudo crear build\launcher
echo ========================================
echo.
exit /b 1


:ERROR_RC

echo.
echo ========================================
echo ERROR: No se pudieron compilar recursos
echo ========================================
echo.
echo Verifique:
echo.
echo   resources\foodflow_launcher.rc
echo   resources\foodflow.ico
echo.
exit /b 1


:ERROR_BUILD

echo.
echo ========================================
echo ERROR: No se pudo compilar FoodFlow.exe
echo ========================================
echo.
exit /b !BUILD_RESULT!


:ERROR_EXE

echo.
echo ========================================
echo ERROR: FoodFlow.exe no fue generado
echo ========================================
echo.
exit /b 1