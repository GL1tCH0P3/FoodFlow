@echo off
setlocal EnableExtensions DisableDelayedExpansion

REM ============================================================
REM FOODFLOW - RUN
REM
REM Responsabilidad:
REM   - Ubicarse en la raiz del proyecto.
REM   - Cargar configuracion local de PostgreSQL.
REM   - Validar variables obligatorias.
REM   - Compilar solamente si foodflow.exe no existe.
REM   - Ejecutar FoodFlow.
REM   - Propagar el codigo de salida de la aplicacion.
REM
REM NO instala dependencias.
REM NO ejecuta setup automaticamente.
REM ============================================================

cd /d "%~dp0"

set "ROOT=%CD%"
set "ENV_FILE=%ROOT%\config\database.env"
set "EXE=%ROOT%\dist\foodflow.exe"


REM ============================================================
REM 1. VERIFICAR CONFIGURACION
REM ============================================================

if not exist "%ENV_FILE%" goto ERROR_ENV_NOT_FOUND


REM ============================================================
REM 2. LIMPIAR VARIABLES ANTERIORES
REM
REM Evita que configuraciones heredadas del sistema sustituyan
REM accidentalmente valores faltantes en database.env.
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
REM Formato esperado:
REM
REM FOODFLOW_DB_HOST=...
REM FOODFLOW_DB_PORT=5432
REM FOODFLOW_DB_NAME=...
REM FOODFLOW_DB_USER=...
REM FOODFLOW_DB_PASSWORD=...
REM FOODFLOW_DB_SSLMODE=require
REM
REM No usar espacios alrededor del signo =
REM No envolver los valores entre comillas.
REM ============================================================

for /f "usebackq eol=# tokens=1,* delims==" %%A in ("%ENV_FILE%") do (
    if /I "%%A"=="FOODFLOW_DB_HOST" set "FOODFLOW_DB_HOST=%%B"
    if /I "%%A"=="FOODFLOW_DB_PORT" set "FOODFLOW_DB_PORT=%%B"
    if /I "%%A"=="FOODFLOW_DB_NAME" set "FOODFLOW_DB_NAME=%%B"
    if /I "%%A"=="FOODFLOW_DB_USER" set "FOODFLOW_DB_USER=%%B"
    if /I "%%A"=="FOODFLOW_DB_PASSWORD" set "FOODFLOW_DB_PASSWORD=%%B"
    if /I "%%A"=="FOODFLOW_DB_SSLMODE" set "FOODFLOW_DB_SSLMODE=%%B"
)


REM ============================================================
REM 4. VALIDAR CONFIGURACION MINIMA
REM
REM PORT y SSLMODE pueden ser opcionales porque ConfiguracionBD
REM ya dispone de valores predeterminados.
REM ============================================================

if not defined FOODFLOW_DB_HOST goto ERROR_CONFIG
if not defined FOODFLOW_DB_NAME goto ERROR_CONFIG
if not defined FOODFLOW_DB_USER goto ERROR_CONFIG
if not defined FOODFLOW_DB_PASSWORD goto ERROR_CONFIG


REM ============================================================
REM 5. VERIFICAR EJECUTABLE
REM ============================================================

if exist "%EXE%" goto EXECUTE


echo.
echo ========================================
echo FOODFLOW NO ESTA COMPILADO
echo ========================================
echo.
echo Intentando compilar...
echo.

call "%ROOT%\scripts\build.bat"

if errorlevel 1 goto ERROR_BUILD

if not exist "%EXE%" goto ERROR_EXE


REM ============================================================
REM 6. EJECUTAR
REM ============================================================

:EXECUTE

cls

"%EXE%"

set "APP_EXIT_CODE=%ERRORLEVEL%"


REM ============================================================
REM 7. RESULTADO
REM ============================================================

echo.

if "%APP_EXIT_CODE%"=="0" goto NORMAL_EXIT


echo.
echo ========================================
echo FOODFLOW FINALIZO CON ERROR
echo ========================================
echo.
echo Codigo de salida:
echo %APP_EXIT_CODE%
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
echo %ENV_FILE%
echo.
echo Para desarrollo ejecute primero:
echo.
echo   scripts\setup.bat
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
echo %EXE%
echo.
pause
exit /b 1