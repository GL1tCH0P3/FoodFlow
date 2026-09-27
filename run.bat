@echo off
setlocal EnableExtensions DisableDelayedExpansion

REM ============================================================
REM FOODFLOW - RUN
REM
REM Responsabilidad:
REM   - Ubicarse en la raiz del proyecto.
REM   - Cargar configuracion PostgreSQL.
REM   - Validar variables obligatorias.
REM   - Ejecutar directamente dist\foodflow.exe cuando existe.
REM   - Compilar solo si el proyecto ya tiene .deps preparado.
REM   - Propagar el codigo de salida.
REM
REM IMPORTANTE:
REM   Una distribucion preparada con dist\foodflow.exe NO
REM   requiere MSVC para ejecutarse.
REM
REM NO instala dependencias.
REM NO ejecuta setup automaticamente.
REM ============================================================

cd /d "%~dp0"

set "ROOT=%CD%"

set "ENV_FILE=%ROOT%\config\database.env"

set "EXE=%ROOT%\dist\foodflow.exe"

set "DEPS_MARKER=%ROOT%\.deps\x64-windows\include\pqxx"


REM ============================================================
REM 1. VERIFICAR CONFIGURACION
REM ============================================================

if not exist "%ENV_FILE%" goto ERROR_ENV_NOT_FOUND


REM ============================================================
REM 2. LIMPIAR VARIABLES ANTERIORES
REM
REM Evita que configuraciones heredadas del sistema sustituyan
REM valores faltantes dentro de database.env.
REM ============================================================

set "FOODFLOW_DB_HOST="

set "FOODFLOW_DB_PORT="

set "FOODFLOW_DB_NAME="

set "FOODFLOW_DB_USER="

set "FOODFLOW_DB_PASSWORD="

set "FOODFLOW_DB_SSLMODE="


REM ============================================================
REM 3. CARGAR DATABASE.ENV
REM
REM Formato:
REM
REM FOODFLOW_DB_HOST=...
REM FOODFLOW_DB_PORT=5432
REM FOODFLOW_DB_NAME=...
REM FOODFLOW_DB_USER=...
REM FOODFLOW_DB_PASSWORD=...
REM FOODFLOW_DB_SSLMODE=require
REM
REM No usar espacios alrededor de =
REM No envolver los valores entre comillas.
REM ============================================================

for /f "usebackq eol=# tokens=1,* delims==" %%A in ("%ENV_FILE%") do (

    if /I "%%A"=="FOODFLOW_DB_HOST" (
        set "FOODFLOW_DB_HOST=%%B"
    )

    if /I "%%A"=="FOODFLOW_DB_PORT" (
        set "FOODFLOW_DB_PORT=%%B"
    )

    if /I "%%A"=="FOODFLOW_DB_NAME" (
        set "FOODFLOW_DB_NAME=%%B"
    )

    if /I "%%A"=="FOODFLOW_DB_USER" (
        set "FOODFLOW_DB_USER=%%B"
    )

    if /I "%%A"=="FOODFLOW_DB_PASSWORD" (
        set "FOODFLOW_DB_PASSWORD=%%B"
    )

    if /I "%%A"=="FOODFLOW_DB_SSLMODE" (
        set "FOODFLOW_DB_SSLMODE=%%B"
    )
)


REM ============================================================
REM 4. VALIDAR CONFIGURACION
REM
REM PORT y SSLMODE son opcionales porque ConfiguracionBD
REM dispone de valores predeterminados.
REM ============================================================

if not defined FOODFLOW_DB_HOST goto ERROR_CONFIG

if not defined FOODFLOW_DB_NAME goto ERROR_CONFIG

if not defined FOODFLOW_DB_USER goto ERROR_CONFIG

if not defined FOODFLOW_DB_PASSWORD goto ERROR_CONFIG


REM ============================================================
REM 5. VERIFICAR EJECUTABLE
REM ============================================================

if exist "%EXE%" goto VERIFY_RUNTIME


REM ------------------------------------------------------------
REM No hay ejecutable.
REM
REM Si .deps tampoco existe, estamos probablemente frente a
REM un clon limpio y primero debe ejecutarse setup.bat.
REM ------------------------------------------------------------

if not exist "%DEPS_MARKER%\" goto ERROR_SETUP_REQUIRED


echo.
echo ========================================
echo FOODFLOW NO ESTA COMPILADO
echo ========================================
echo.
echo Las dependencias ya estan preparadas.
echo Se compilara FoodFlow...
echo.


call "%ROOT%\scripts\build.bat"


if errorlevel 1 goto ERROR_BUILD


if not exist "%EXE%" goto ERROR_EXE


REM ============================================================
REM 6. VERIFICAR RUNTIME
REM ============================================================

:VERIFY_RUNTIME


REM libpq.dll forma parte de nuestra distribucion PostgreSQL.

if not exist "%ROOT%\dist\libpq.dll" goto ERROR_RUNTIME


REM Añadimos dist al PATH del proceso por seguridad.
REM El cambio solo existe durante esta ejecucion.

set "PATH=%ROOT%\dist;%PATH%"


REM ============================================================
REM 7. EJECUTAR
REM ============================================================

:EXECUTE

cls


"%EXE%"


set "APP_EXIT_CODE=%ERRORLEVEL%"


REM ============================================================
REM 8. RESULTADO
REM ============================================================

echo.


if "%APP_EXIT_CODE%"=="0" goto NORMAL_EXIT


echo.
echo ========================================
echo FOODFLOW FINALIZO CON ERROR
echo ========================================
echo.
echo Codigo de salida:
echo.
echo   %APP_EXIT_CODE%
echo.

pause

exit /b %APP_EXIT_CODE%


:NORMAL_EXIT

echo.

pause

exit /b 0


REM ============================================================
REM ERRORES
REM ============================================================

:ERROR_ENV_NOT_FOUND

echo.
echo ========================================
echo ERROR: CONFIGURACION NO ENCONTRADA
echo ========================================
echo.
echo No existe:
echo.
echo   %ENV_FILE%
echo.
echo Si es la primera ejecucion de un clon de Git:
echo.
echo   scripts\setup.bat
echo.
echo Luego complete:
echo.
echo   config\database.env
echo.
pause
exit /b 1


:ERROR_CONFIG

echo.
echo ========================================
echo ERROR: CONFIGURACION INCOMPLETA
echo ========================================
echo.
echo Revise:
echo.
echo   config\database.env
echo.
echo Son obligatorias:
echo.
echo   FOODFLOW_DB_HOST
echo   FOODFLOW_DB_NAME
echo   FOODFLOW_DB_USER
echo   FOODFLOW_DB_PASSWORD
echo.
echo Opcionales:
echo.
echo   FOODFLOW_DB_PORT
echo   FOODFLOW_DB_SSLMODE
echo.
pause
exit /b 1


:ERROR_SETUP_REQUIRED

echo.
echo ========================================
echo FOODFLOW REQUIERE PREPARACION INICIAL
echo ========================================
echo.
echo No existe:
echo.
echo   dist\foodflow.exe
echo.
echo y tampoco se encontraron las dependencias
echo locales en .deps.
echo.
echo Ejecute una sola vez:
echo.
echo   scripts\setup.bat
echo.
echo Despues utilice normalmente:
echo.
echo   run.bat
echo.
pause
exit /b 1


:ERROR_BUILD

echo.
echo ========================================
echo ERROR: NO FUE POSIBLE COMPILAR FOODFLOW
echo ========================================
echo.
echo Revise los errores mostrados por build.bat.
echo.
pause
exit /b 1


:ERROR_EXE

echo.
echo ========================================
echo ERROR: EJECUTABLE NO GENERADO
echo ========================================
echo.
echo La compilacion termino pero no se encontro:
echo.
echo   %EXE%
echo.
pause
exit /b 1


:ERROR_RUNTIME

echo.
echo ========================================
echo ERROR: DISTRIBUCION INCOMPLETA
echo ========================================
echo.
echo Se encontro foodflow.exe pero falta:
echo.
echo   dist\libpq.dll
echo.
echo En la maquina de desarrollo ejecute:
echo.
echo   scripts\build.bat
echo.
echo y vuelva a preparar la entrega.
echo.
pause
exit /b 1