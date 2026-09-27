@echo off
setlocal EnableExtensions DisableDelayedExpansion

REM ============================================================
REM FOODFLOW - PAQUETE PARA EL PROFESOR
REM
REM Genera:
REM
REM   release\FoodFlow-entrega-profesor.zip
REM
REM El paquete contiene:
REM   - Codigo fuente.
REM   - Scripts.
REM   - Dear ImGui.
REM   - Paquete vendor para recompilar.
REM   - dist\foodflow.exe.
REM   - DLL runtime.
REM   - database.env REAL.
REM   - database.env.example.
REM
REM El paquete NO contiene:
REM   - .git
REM   - .deps
REM   - build
REM   - archivos temporales
REM   - facturas generadas
REM
REM IMPORTANTE:
REM Este ZIP contiene credenciales reales.
REM NO debe publicarse en Git.
REM ============================================================


cd /d "%~dp0.."


set "ROOT=%CD%"

set "RELEASE_DIR=%ROOT%\release"

set "STAGING_PARENT=%RELEASE_DIR%\_staging"

set "STAGING_PROJECT=%STAGING_PARENT%\FoodFlow"

set "ZIP_FILE=%RELEASE_DIR%\FoodFlow-entrega-profesor.zip"

set "ENV_FILE=%ROOT%\config\database.env"

set "ENV_EXAMPLE=%ROOT%\config\database.env.example"

set "VENDOR_ZIP=%ROOT%\vendor\foodflow-deps-win64.zip"

set "EXE=%ROOT%\dist\foodflow.exe"


echo.
echo ========================================
echo   FOODFLOW - PAQUETE PARA PROFESOR
echo ========================================
echo.


REM ============================================================
REM 1. VERIFICAR CONFIGURACION REAL
REM ============================================================

echo [1/6] Verificando configuracion...


if not exist "%ENV_FILE%" goto ERROR_ENV


if not exist "%ENV_EXAMPLE%" goto ERROR_ENV_EXAMPLE


REM ------------------------------------------------------------
REM Comprobar que las variables obligatorias tengan contenido.
REM No mostramos sus valores.
REM ------------------------------------------------------------

findstr /R /C:"^FOODFLOW_DB_HOST=." "%ENV_FILE%" >nul

if errorlevel 1 goto ERROR_ENV_VALUES


findstr /R /C:"^FOODFLOW_DB_NAME=." "%ENV_FILE%" >nul

if errorlevel 1 goto ERROR_ENV_VALUES


findstr /R /C:"^FOODFLOW_DB_USER=." "%ENV_FILE%" >nul

if errorlevel 1 goto ERROR_ENV_VALUES


findstr /R /C:"^FOODFLOW_DB_PASSWORD=." "%ENV_FILE%" >nul

if errorlevel 1 goto ERROR_ENV_VALUES


echo       database.env encontrado.

echo       Credenciales obligatorias configuradas.


REM ============================================================
REM 2. VERIFICAR VENDOR
REM ============================================================

echo.
echo [2/6] Verificando dependencias reproducibles...


if not exist "%VENDOR_ZIP%" goto ERROR_VENDOR


if not exist "%ROOT%\vendor\imgui\imgui.h" goto ERROR_IMGUI


echo       vendor\foodflow-deps-win64.zip encontrado.

echo       Dear ImGui encontrado.


REM ============================================================
REM 3. BUILD LIMPIO
REM
REM Siempre generamos el paquete a partir de una compilacion
REM actual, evitando distribuir un .exe antiguo.
REM ============================================================

echo.
echo [3/6] Compilando version final...
echo.


call "%ROOT%\scripts\build.bat"


if errorlevel 1 goto ERROR_BUILD


if not exist "%EXE%" goto ERROR_EXE


if not exist "%ROOT%\dist\libpq.dll" goto ERROR_RUNTIME


echo.
echo       Distribucion runtime preparada.


REM ============================================================
REM 4. PREPARAR STAGING
REM ============================================================

echo.
echo [4/6] Preparando contenido de entrega...


if not exist "%RELEASE_DIR%\" (
    mkdir "%RELEASE_DIR%"
)


if exist "%STAGING_PARENT%\" (
    rmdir /S /Q "%STAGING_PARENT%"
)


if exist "%STAGING_PARENT%\" goto ERROR_STAGING_CLEAN


mkdir "%STAGING_PROJECT%"


if errorlevel 1 goto ERROR_STAGING_CREATE


REM ------------------------------------------------------------
REM Copiar proyecto excluyendo artefactos locales.
REM
REM Se copia TODO lo demas para no perder documentacion,
REM scripts SQL u otros archivos academicos que existan
REM en el proyecto.
REM ------------------------------------------------------------

set "FOODFLOW_PACKAGE_ROOT=%ROOT%"

set "FOODFLOW_PACKAGE_STAGE=%STAGING_PROJECT%"


powershell.exe ^
    -NoProfile ^
    -ExecutionPolicy Bypass ^
    -Command ^
    "$excludeDirs = @('.git','.deps','.deps_tmp','build','release','facturas','.vs','.idea','.vendor-src','.vendor-stage','vendor-package','.tools','vcpkg_installed','bin','obj','out','Debug','Release','x64','x86'); $excludeExt = @('.log','.tmp','.temp','.obj','.pdb','.ilk'); Get-ChildItem -LiteralPath $env:FOODFLOW_PACKAGE_ROOT -Force | Where-Object { if ($_.PSIsContainer) { $excludeDirs -notcontains $_.Name } else { $excludeExt -notcontains $_.Extension.ToLowerInvariant() } } | ForEach-Object { Copy-Item -LiteralPath $_.FullName -Destination (Join-Path $env:FOODFLOW_PACKAGE_STAGE $_.Name) -Recurse -Force -ErrorAction Stop }"


if errorlevel 1 goto ERROR_COPY_PROJECT


REM ------------------------------------------------------------
REM Crear carpeta facturas limpia.
REM ------------------------------------------------------------

if not exist "%STAGING_PROJECT%\facturas\" (
    mkdir "%STAGING_PROJECT%\facturas"
)


if exist "%ROOT%\facturas\.gitkeep" (

    copy /Y ^
        "%ROOT%\facturas\.gitkeep" ^
        "%STAGING_PROJECT%\facturas\.gitkeep" ^
        >nul
)


REM ------------------------------------------------------------
REM Verificaciones sobre el staging final.
REM ------------------------------------------------------------

if not exist "%STAGING_PROJECT%\run.bat" goto ERROR_STAGING_CONTENT

if not exist "%STAGING_PROJECT%\src\" goto ERROR_STAGING_CONTENT

if not exist "%STAGING_PROJECT%\scripts\" goto ERROR_STAGING_CONTENT

if not exist "%STAGING_PROJECT%\vendor\foodflow-deps-win64.zip" goto ERROR_STAGING_CONTENT

if not exist "%STAGING_PROJECT%\dist\foodflow.exe" goto ERROR_STAGING_CONTENT

if not exist "%STAGING_PROJECT%\dist\libpq.dll" goto ERROR_STAGING_CONTENT

if not exist "%STAGING_PROJECT%\config\database.env" goto ERROR_STAGING_CONTENT

if not exist "%STAGING_PROJECT%\config\database.env.example" goto ERROR_STAGING_CONTENT


echo       Proyecto preparado.

echo       .deps excluido.

echo       build excluido.

echo       facturas generadas excluidas.

echo       dist incluido.

echo       vendor incluido.

echo       database.env real incluido.


REM ============================================================
REM 5. COMPRIMIR
REM ============================================================

echo.
echo [5/6] Creando archivo ZIP...


if exist "%ZIP_FILE%" (
    del /Q "%ZIP_FILE%"
)


set "FOODFLOW_PACKAGE_SOURCE=%STAGING_PARENT%"

set "FOODFLOW_PACKAGE_ZIP=%ZIP_FILE%"


powershell.exe ^
    -NoProfile ^
    -ExecutionPolicy Bypass ^
    -Command ^
    "Add-Type -AssemblyName System.IO.Compression.FileSystem; if (Test-Path -LiteralPath $env:FOODFLOW_PACKAGE_ZIP) { Remove-Item -LiteralPath $env:FOODFLOW_PACKAGE_ZIP -Force }; [System.IO.Compression.ZipFile]::CreateFromDirectory($env:FOODFLOW_PACKAGE_SOURCE, $env:FOODFLOW_PACKAGE_ZIP, [System.IO.Compression.CompressionLevel]::Optimal, $false)"


if errorlevel 1 goto ERROR_ZIP


if not exist "%ZIP_FILE%" goto ERROR_ZIP


echo       ZIP generado correctamente.


REM ============================================================
REM 6. FINALIZAR
REM ============================================================

echo.
echo [6/6] Calculando integridad...


echo.
echo SHA-256:
echo.


powershell.exe ^
    -NoProfile ^
    -ExecutionPolicy Bypass ^
    -Command ^
    "(Get-FileHash -Algorithm SHA256 -LiteralPath $env:FOODFLOW_PACKAGE_ZIP).Hash"


echo.


for %%A in ("%ZIP_FILE%") do (

    echo Tamano:
    echo   %%~zA bytes
)


REM ------------------------------------------------------------
REM El staging ya no es necesario.
REM ------------------------------------------------------------

if exist "%STAGING_PARENT%\" (
    rmdir /S /Q "%STAGING_PARENT%"
)


echo.
echo ========================================
echo       PAQUETE GENERADO CORRECTAMENTE
echo ========================================
echo.
echo Archivo:
echo.
echo   %ZIP_FILE%
echo.
echo Contiene:
echo.
echo   - Codigo fuente completo
echo   - Dear ImGui
echo   - Dependencias vendor comprimidas
echo   - foodflow.exe
echo   - DLL runtime
echo   - database.env REAL
echo   - database.env.example
echo.
echo El profesor puede ejecutar:
echo.
echo   run.bat
echo.
echo sin compilar nuevamente.
echo.
echo ========================================
echo ADVERTENCIA DE SEGURIDAD
echo ========================================
echo.
echo Este ZIP contiene credenciales reales.
echo.
echo NO subir:
echo.
echo   release\FoodFlow-entrega-profesor.zip
echo.
echo a GitHub, Drive publico u otros repositorios
echo publicos.
echo.

pause

exit /b 0


REM ============================================================
REM ERRORES
REM ============================================================

:ERROR_ENV

echo.
echo ========================================
echo ERROR: database.env no encontrado
echo ========================================
echo.
echo Para crear la entrega al profesor debe existir:
echo.
echo   config\database.env
echo.
echo con las credenciales reales.
echo.
pause
exit /b 1


:ERROR_ENV_EXAMPLE

echo.
echo ========================================
echo ERROR: database.env.example no encontrado
echo ========================================
echo.
pause
exit /b 1


:ERROR_ENV_VALUES

echo.
echo ========================================
echo ERROR: database.env incompleto
echo ========================================
echo.
echo Verifique las variables obligatorias:
echo.
echo   FOODFLOW_DB_HOST
echo   FOODFLOW_DB_NAME
echo   FOODFLOW_DB_USER
echo   FOODFLOW_DB_PASSWORD
echo.
echo Los valores NO se mostraron por seguridad.
echo.
pause
exit /b 1


:ERROR_VENDOR

echo.
echo ========================================
echo ERROR: Paquete vendor no encontrado
echo ========================================
echo.
echo Falta:
echo.
echo   vendor\foodflow-deps-win64.zip
echo.
pause
exit /b 1


:ERROR_IMGUI

echo.
echo ========================================
echo ERROR: Dear ImGui no encontrado
echo ========================================
echo.
pause
exit /b 1


:ERROR_BUILD

echo.
echo ========================================
echo ERROR: No fue posible compilar FoodFlow
echo ========================================
echo.
echo La entrega no sera generada para evitar
echo distribuir un ejecutable desactualizado.
echo.
pause
exit /b 1


:ERROR_EXE

echo.
echo ========================================
echo ERROR: foodflow.exe no fue generado
echo ========================================
echo.
pause
exit /b 1


:ERROR_RUNTIME

echo.
echo ========================================
echo ERROR: Runtime PostgreSQL incompleto
echo ========================================
echo.
echo Falta:
echo.
echo   dist\libpq.dll
echo.
pause
exit /b 1


:ERROR_STAGING_CLEAN

echo.
echo ========================================
echo ERROR: No fue posible limpiar staging
echo ========================================
echo.
pause
exit /b 1


:ERROR_STAGING_CREATE

echo.
echo ========================================
echo ERROR: No fue posible crear staging
echo ========================================
echo.
pause
exit /b 1


:ERROR_COPY_PROJECT

echo.
echo ========================================
echo ERROR: No fue posible copiar el proyecto
echo ========================================
echo.


if exist "%STAGING_PARENT%\" (
    rmdir /S /Q "%STAGING_PARENT%"
)


pause
exit /b 1


:ERROR_STAGING_CONTENT

echo.
echo ========================================
echo ERROR: Paquete incompleto
echo ========================================
echo.
echo El staging no contiene todos los archivos
echo necesarios para ejecutar o reconstruir FoodFlow.
echo.


if exist "%STAGING_PARENT%\" (
    rmdir /S /Q "%STAGING_PARENT%"
)


pause
exit /b 1


:ERROR_ZIP

echo.
echo ========================================
echo ERROR: No fue posible generar el ZIP
echo ========================================
echo.


if exist "%STAGING_PARENT%\" (
    rmdir /S /Q "%STAGING_PARENT%"
)


pause
exit /b 1