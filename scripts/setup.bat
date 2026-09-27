@echo off
setlocal EnableExtensions EnableDelayedExpansion

REM ============================================================
REM FOODFLOW - SETUP
REM
REM Preparacion inicial del proyecto.
REM
REM Se ejecuta una sola vez despues de clonar o descomprimir
REM el repositorio.
REM
REM Responsabilidad:
REM   - Verificar MSVC C++ x64.
REM   - Verificar paquete vendor.
REM   - Extraer dependencias precompiladas.
REM   - Detectar cambios mediante SHA-256.
REM   - Crear database.env local si no existe.
REM   - Compilar la aplicacion FoodFlow.
REM   - Compilar el launcher FoodFlow.exe.
REM
REM NO usa vcpkg.
REM NO usa CMake.
REM NO compila libpq.
REM NO compila libpqxx.
REM NO descarga dependencias.
REM ============================================================

cd /d "%~dp0.."

set "ROOT=%CD%"

set "VENDOR_ZIP=!ROOT!\vendor\foodflow-deps-win64.zip"

set "DEPS_DIR=!ROOT!\.deps"

set "DEPS_TMP=!ROOT!\.deps_tmp"

set "DEPS_HASH_FILE=!DEPS_DIR!\.vendor.sha256"

set "TRIPLET_DIR=!DEPS_DIR!\x64-windows"

set "INCLUDE_DIR=!TRIPLET_DIR!\include"

set "LIB_DIR=!TRIPLET_DIR!\lib"

set "BIN_DIR=!TRIPLET_DIR!\bin"

set "CONFIG_EXAMPLE=!ROOT!\config\database.env.example"

set "CONFIG_REAL=!ROOT!\config\database.env"

set "CONFIG_CREATED=0"


echo.
echo ========================================
echo          FOODFLOW - SETUP
echo ========================================
echo.


REM ============================================================
REM 1. VERIFICAR TOOLCHAIN
REM ============================================================

echo [1/6] Verificando compilador C++...


call "!ROOT!\scripts\msvc_env.bat"


if errorlevel 1 goto ERROR_MSVC


echo       Visual Studio:
echo       !FOODFLOW_VS_PATH!
echo.
echo       MSVC:
echo       !FOODFLOW_MSVC_VERSION!
echo.
echo       Arquitectura: x64
echo       Compilador preparado correctamente.


REM ============================================================
REM 2. VERIFICAR PAQUETE VENDOR
REM ============================================================

echo.
echo [2/6] Verificando paquete de dependencias...


if not exist "!VENDOR_ZIP!" goto ERROR_VENDOR


set "HASH_FILE=!TEMP!\foodflow_hash_!RANDOM!_!RANDOM!.txt"


powershell.exe ^
    -NoProfile ^
    -ExecutionPolicy Bypass ^
    -Command ^
    "(Get-FileHash -Algorithm SHA256 -LiteralPath $env:VENDOR_ZIP).Hash.ToLower()" ^
    > "!HASH_FILE!" ^
    2>nul


set "ZIP_HASH="


if exist "!HASH_FILE!" (
    set /p ZIP_HASH=<"!HASH_FILE!"
)


del /Q "!HASH_FILE!" >nul 2>&1


if not defined ZIP_HASH goto ERROR_HASH


echo       Archivo:
echo       vendor\foodflow-deps-win64.zip
echo.
echo       SHA-256:
echo       !ZIP_HASH!
echo.
echo       Paquete encontrado correctamente.


REM ============================================================
REM 3. PREPARAR DEPENDENCIAS
REM ============================================================

echo.
echo [3/6] Verificando dependencias locales...


set "INSTALLED_HASH="


if exist "!DEPS_HASH_FILE!" (
    set /p INSTALLED_HASH=<"!DEPS_HASH_FILE!"
)


set "DEPS_OK=1"


REM ------------------------------------------------------------
REM Headers libpqxx
REM ------------------------------------------------------------

if not exist "!INCLUDE_DIR!\pqxx\" (
    set "DEPS_OK=0"
)


REM ------------------------------------------------------------
REM Biblioteca libpq
REM ------------------------------------------------------------

if not exist "!LIB_DIR!\libpq.lib" (
    set "DEPS_OK=0"
)


REM ------------------------------------------------------------
REM Runtime libpq
REM ------------------------------------------------------------

if not exist "!BIN_DIR!\libpq.dll" (
    set "DEPS_OK=0"
)


REM ------------------------------------------------------------
REM Biblioteca libpqxx
REM ------------------------------------------------------------

set "LOCAL_PQXX_LIB="

set "PQXX_CHECK_FILE=!TEMP!\foodflow_pqxx_check_!RANDOM!_!RANDOM!.txt"


if exist "!LIB_DIR!\" (

    dir /b /a-d ^
        "!LIB_DIR!\*pqxx*.lib" ^
        > "!PQXX_CHECK_FILE!" ^
        2>nul
)


if exist "!PQXX_CHECK_FILE!" (
    set /p LOCAL_PQXX_LIB=<"!PQXX_CHECK_FILE!"
)


del /Q "!PQXX_CHECK_FILE!" >nul 2>&1


if not defined LOCAL_PQXX_LIB (
    set "DEPS_OK=0"
)


REM ------------------------------------------------------------
REM Verificar hash del paquete vendor
REM ------------------------------------------------------------

if /I not "!INSTALLED_HASH!"=="!ZIP_HASH!" (
    set "DEPS_OK=0"
)


if "!DEPS_OK!"=="1" goto DEPS_ALREADY_READY


REM ============================================================
REM EXTRAER DEPENDENCIAS EN DIRECTORIO TEMPORAL
REM ============================================================

echo       Preparando dependencias...


if exist "!DEPS_TMP!\" (
    rmdir /S /Q "!DEPS_TMP!"
)


mkdir "!DEPS_TMP!"


if errorlevel 1 goto ERROR_TEMP_DIR


powershell.exe ^
    -NoProfile ^
    -ExecutionPolicy Bypass ^
    -Command ^
    "Expand-Archive -LiteralPath $env:VENDOR_ZIP -DestinationPath $env:DEPS_TMP -Force"


if errorlevel 1 goto ERROR_EXTRACT


REM ============================================================
REM VALIDAR CONTENIDO DEL PAQUETE
REM ============================================================

if not exist "!DEPS_TMP!\x64-windows\include\pqxx\" (
    goto ERROR_VENDOR_HEADERS
)


if not exist "!DEPS_TMP!\x64-windows\lib\libpq.lib" (
    goto ERROR_VENDOR_LIBPQ
)


if not exist "!DEPS_TMP!\x64-windows\bin\libpq.dll" (
    goto ERROR_VENDOR_LIBPQ_RUNTIME
)


set "TEMP_PQXX_LIB="

set "TEMP_PQXX_FILE=!TEMP!\foodflow_pqxx_vendor_!RANDOM!_!RANDOM!.txt"


dir /b /a-d ^
    "!DEPS_TMP!\x64-windows\lib\*pqxx*.lib" ^
    > "!TEMP_PQXX_FILE!" ^
    2>nul


if exist "!TEMP_PQXX_FILE!" (
    set /p TEMP_PQXX_LIB=<"!TEMP_PQXX_FILE!"
)


del /Q "!TEMP_PQXX_FILE!" >nul 2>&1


if not defined TEMP_PQXX_LIB goto ERROR_VENDOR_PQXX


REM ============================================================
REM INSTALAR DEPENDENCIAS LOCALES
REM ============================================================

if exist "!DEPS_DIR!\" (
    rmdir /S /Q "!DEPS_DIR!"
)


if exist "!DEPS_DIR!\" goto ERROR_MOVE_DEPS


move "!DEPS_TMP!" "!DEPS_DIR!" >nul


if errorlevel 1 goto ERROR_MOVE_DEPS


> "!DEPS_HASH_FILE!" echo !ZIP_HASH!


echo       Dependencias preparadas correctamente.


goto DEPS_READY


:DEPS_ALREADY_READY

echo       Dependencias actualizadas.
echo       No es necesario extraer nuevamente.


:DEPS_READY


REM ============================================================
REM 4. CONFIGURACION LOCAL
REM ============================================================

echo.
echo [4/6] Preparando configuracion local...


if not exist "!ROOT!\config\" (
    mkdir "!ROOT!\config"
)


if exist "!CONFIG_REAL!" goto CONFIG_EXISTS


if not exist "!CONFIG_EXAMPLE!" goto ERROR_CONFIG_EXAMPLE


copy /Y ^
    "!CONFIG_EXAMPLE!" ^
    "!CONFIG_REAL!" ^
    >nul


if errorlevel 1 goto ERROR_CONFIG_COPY


set "CONFIG_CREATED=1"


echo       Se creo:
echo       config\database.env
echo.
echo       Complete sus credenciales de PostgreSQL.


goto CONFIG_READY


:CONFIG_EXISTS

echo       config\database.env ya existe.
echo       Se conserva sin modificaciones.


:CONFIG_READY


REM ============================================================
REM 5. COMPILAR APLICACION
REM ============================================================

echo.
echo [5/6] Compilando aplicacion FoodFlow...
echo.


call "!ROOT!\scripts\build.bat"


if errorlevel 1 goto ERROR_BUILD


if not exist "!ROOT!\dist\foodflow.exe" (
    goto ERROR_BUILD_EXE
)


echo.
echo       Aplicacion compilada correctamente.


REM ============================================================
REM 6. COMPILAR LAUNCHER
REM ============================================================

echo.
echo [6/6] Generando FoodFlow.exe...
echo.


call "!ROOT!\scripts\build_launcher.bat"


if errorlevel 1 goto ERROR_LAUNCHER


if not exist "!ROOT!\FoodFlow.exe" (
    goto ERROR_LAUNCHER_EXE
)


echo.
echo       Launcher generado correctamente.


REM ============================================================
REM RESULTADO
REM ============================================================

echo.
echo ========================================
echo          FOODFLOW LISTO
echo ========================================
echo.
echo Entorno:
echo.
echo   C++:        C++17
echo   Compilador: MSVC !FOODFLOW_MSVC_VERSION!
echo   Plataforma: Windows x64
echo.
echo Aplicacion:
echo.
echo   dist\foodflow.exe
echo.
echo Launcher principal:
echo.
echo   FoodFlow.exe
echo.


if "!CONFIG_CREATED!"=="1" (

    echo ========================================
    echo CONFIGURACION PENDIENTE
    echo ========================================
    echo.
    echo Se genero:
    echo.
    echo   config\database.env
    echo.
    echo usando:
    echo.
    echo   config\database.env.example
    echo.
    echo Antes de abrir FoodFlow.exe complete:
    echo.
    echo   FOODFLOW_DB_HOST
    echo   FOODFLOW_DB_NAME
    echo   FOODFLOW_DB_USER
    echo   FOODFLOW_DB_PASSWORD
    echo.
)


echo ========================================
echo PRIMERA EJECUCION COMPLETADA
echo ========================================
echo.
echo A partir de ahora puede iniciar FoodFlow
echo haciendo doble clic directamente sobre:
echo.
echo   FoodFlow.exe
echo.
echo No es necesario ejecutar setup.bat
echo nuevamente para utilizar la aplicacion.
echo.
echo Para desarrollo:
echo.
echo   Compilar aplicacion:
echo       scripts\build.bat
echo.
echo   Reconstruir launcher:
echo       scripts\build_launcher.bat
echo.
echo   Ejecucion tecnica alternativa:
echo       run.bat
echo.

pause

exit /b 0


REM ============================================================
REM ERRORES
REM ============================================================

:ERROR_MSVC

echo.
echo ========================================
echo ERROR: Toolchain C++ no disponible
echo ========================================
echo.
echo FoodFlow requiere Visual Studio Build Tools
echo con soporte para C++ x64 para COMPILAR.
echo.
pause
exit /b 1


:ERROR_VENDOR

echo.
echo ========================================
echo ERROR: Paquete vendor no encontrado
echo ========================================
echo.
echo Archivo esperado:
echo.
echo   !VENDOR_ZIP!
echo.
pause
exit /b 1


:ERROR_HASH

echo.
echo ========================================
echo ERROR: No se pudo calcular SHA-256
echo ========================================
echo.
pause
exit /b 1


:ERROR_TEMP_DIR

echo.
echo ========================================
echo ERROR: No se pudo crear .deps_tmp
echo ========================================
echo.
pause
exit /b 1


:ERROR_EXTRACT

echo.
echo ========================================
echo ERROR: No se pudo extraer el paquete vendor
echo ========================================
echo.


if exist "!DEPS_TMP!\" (
    rmdir /S /Q "!DEPS_TMP!"
)


pause
exit /b 1


:ERROR_VENDOR_HEADERS

echo.
echo ========================================
echo ERROR: Paquete vendor invalido
echo ========================================
echo.
echo Falta:
echo.
echo   x64-windows\include\pqxx
echo.


if exist "!DEPS_TMP!\" (
    rmdir /S /Q "!DEPS_TMP!"
)


pause
exit /b 1


:ERROR_VENDOR_LIBPQ

echo.
echo ========================================
echo ERROR: Paquete vendor invalido
echo ========================================
echo.
echo Falta:
echo.
echo   x64-windows\lib\libpq.lib
echo.


if exist "!DEPS_TMP!\" (
    rmdir /S /Q "!DEPS_TMP!"
)


pause
exit /b 1


:ERROR_VENDOR_LIBPQ_RUNTIME

echo.
echo ========================================
echo ERROR: Paquete vendor invalido
echo ========================================
echo.
echo Falta la biblioteca runtime:
echo.
echo   x64-windows\bin\libpq.dll
echo.
echo Esta DLL es necesaria para ejecutar FoodFlow.
echo.


if exist "!DEPS_TMP!\" (
    rmdir /S /Q "!DEPS_TMP!"
)


pause
exit /b 1


:ERROR_VENDOR_PQXX

echo.
echo ========================================
echo ERROR: Paquete vendor invalido
echo ========================================
echo.
echo No se encontro la biblioteca libpqxx.
echo.


if exist "!DEPS_TMP!\" (
    rmdir /S /Q "!DEPS_TMP!"
)


pause
exit /b 1


:ERROR_MOVE_DEPS

echo.
echo ========================================
echo ERROR: No se pudo preparar .deps
echo ========================================
echo.


if exist "!DEPS_TMP!\" (
    rmdir /S /Q "!DEPS_TMP!"
)


pause
exit /b 1


:ERROR_CONFIG_EXAMPLE

echo.
echo ========================================
echo ERROR: Configuracion base no encontrada
echo ========================================
echo.
echo Falta:
echo.
echo   config\database.env.example
echo.
pause
exit /b 1


:ERROR_CONFIG_COPY

echo.
echo ========================================
echo ERROR: No se pudo crear database.env
echo ========================================
echo.
pause
exit /b 1


:ERROR_BUILD

echo.
echo ========================================
echo ERROR: La compilacion de FoodFlow fallo
echo ========================================
echo.
echo Revise el primer error mostrado por:
echo.
echo   scripts\build.bat
echo.
pause
exit /b 1


:ERROR_BUILD_EXE

echo.
echo ========================================
echo ERROR: foodflow.exe no fue generado
echo ========================================
echo.
echo Se esperaba:
echo.
echo   dist\foodflow.exe
echo.
pause
exit /b 1


:ERROR_LAUNCHER

echo.
echo ========================================
echo ERROR: El launcher no pudo compilarse
echo ========================================
echo.
echo Revise el primer error mostrado por:
echo.
echo   scripts\build_launcher.bat
echo.
pause
exit /b 1


:ERROR_LAUNCHER_EXE

echo.
echo ========================================
echo ERROR: FoodFlow.exe no fue generado
echo ========================================
echo.
echo Se esperaba:
echo.
echo   !ROOT!\FoodFlow.exe
echo.
pause
exit /b 1