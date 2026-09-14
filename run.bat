@echo off
setlocal EnableExtensions

cd /d "%~dp0"

set "ROOT=%CD%"
set "ENV_FILE=%ROOT%\config\database.env"
set "EXE=%ROOT%\dist\foodflow.exe"

REM ------------------------------------------------------------
REM Validar archivo de configuracion
REM ------------------------------------------------------------

if not exist "%ENV_FILE%" (
    echo.
    echo ERROR: No existe config\database.env
    echo.
    echo Ejecute primero:
    echo scripts\setup.bat
    echo.
    pause
    exit /b 1
)

REM ------------------------------------------------------------
REM Cargar variables del archivo
REM ------------------------------------------------------------

for /f "usebackq eol=# tokens=1,* delims==" %%A in ("%ENV_FILE%") do (
    if not "%%A"=="" (
        set "%%A=%%B"
    )
)

REM ------------------------------------------------------------
REM Validar configuracion minima
REM ------------------------------------------------------------

if "%FOODFLOW_DB_HOST%"=="" goto CONFIG_ERROR
if "%FOODFLOW_DB_NAME%"=="" goto CONFIG_ERROR
if "%FOODFLOW_DB_USER%"=="" goto CONFIG_ERROR
if "%FOODFLOW_DB_PASSWORD%"=="" goto CONFIG_ERROR

REM ------------------------------------------------------------
REM Validar ejecutable
REM ------------------------------------------------------------

if not exist "%EXE%" (
    echo.
    echo FoodFlow no esta compilado.
    echo Compilando...
    echo.

    call "%ROOT%\scripts\build.bat"

    if errorlevel 1 (
        echo.
        echo ERROR: No fue posible compilar FoodFlow.
        pause
        exit /b 1
    )
)

REM ------------------------------------------------------------
REM Ejecutar
REM ------------------------------------------------------------

cls

"%EXE%"

echo.
pause
exit /b 0


:CONFIG_ERROR

echo.
echo ERROR: config\database.env esta incompleto.
echo Complete los datos de conexion antes de ejecutar FoodFlow.
echo.
pause
exit /b 1