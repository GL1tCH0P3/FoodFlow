@echo off

REM ============================================================
REM FOODFLOW - ENTORNO MSVC
REM
REM Localiza Visual Studio Build Tools, detecta el toolset MSVC
REM realmente instalado e inicializa el entorno x64.
REM
REM Este archivo debe ejecutarse mediante CALL.
REM ============================================================


REM ============================================================
REM 1. LOCALIZAR VSWHERE
REM ============================================================

set "FOODFLOW_VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if exist "%FOODFLOW_VSWHERE%" goto VSWHERE_OK

set "FOODFLOW_VSWHERE=%ProgramFiles%\Microsoft Visual Studio\Installer\vswhere.exe"

if exist "%FOODFLOW_VSWHERE%" goto VSWHERE_OK

echo.
echo ========================================
echo ERROR: vswhere.exe no encontrado
echo ========================================
echo.
echo Instale Visual Studio Build Tools con:
echo.
echo   Desktop development with C++
echo.
exit /b 1


:VSWHERE_OK


REM ============================================================
REM 2. LOCALIZAR VISUAL STUDIO CON SOPORTE C++
REM ============================================================

set "FOODFLOW_VS_PATH="
set "FOODFLOW_VS_FILE=%TEMP%\foodflow_vs_%RANDOM%_%RANDOM%.txt"

"%FOODFLOW_VSWHERE%" ^
    -latest ^
    -products * ^
    -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ^
    -property installationPath ^
    > "%FOODFLOW_VS_FILE%" 2>nul

if exist "%FOODFLOW_VS_FILE%" set /p FOODFLOW_VS_PATH=<"%FOODFLOW_VS_FILE%"

del /Q "%FOODFLOW_VS_FILE%" >nul 2>&1

if defined FOODFLOW_VS_PATH goto VS_FOUND

echo.
echo ========================================
echo ERROR: MSVC C++ no encontrado
echo ========================================
echo.
echo Verifique en Visual Studio Installer:
echo.
echo   Desktop development with C++
echo.
exit /b 1


:VS_FOUND


REM ============================================================
REM 3. LOCALIZAR VCVARS64
REM ============================================================

set "FOODFLOW_VCVARS64=%FOODFLOW_VS_PATH%\VC\Auxiliary\Build\vcvars64.bat"

if exist "%FOODFLOW_VCVARS64%" goto VCVARS_FOUND

echo.
echo ========================================
echo ERROR: vcvars64.bat no encontrado
echo ========================================
echo.
echo Ruta esperada:
echo.
echo %FOODFLOW_VCVARS64%
echo.
exit /b 1


:VCVARS_FOUND


REM ============================================================
REM 4. DETECTAR TOOLSET MSVC REALMENTE INSTALADO
REM ============================================================

set "FOODFLOW_MSVC_ROOT=%FOODFLOW_VS_PATH%\VC\Tools\MSVC"

if exist "%FOODFLOW_MSVC_ROOT%\" goto MSVC_ROOT_FOUND

echo.
echo ========================================
echo ERROR: Directorio MSVC no encontrado
echo ========================================
echo.
echo Ruta esperada:
echo.
echo %FOODFLOW_MSVC_ROOT%
echo.
exit /b 1


:MSVC_ROOT_FOUND

set "FOODFLOW_MSVC_VERSION="
set "FOODFLOW_MSVC_FILE=%TEMP%\foodflow_msvc_%RANDOM%_%RANDOM%.txt"

powershell.exe ^
    -NoProfile ^
    -ExecutionPolicy Bypass ^
    -Command ^
    "$root=$env:FOODFLOW_MSVC_ROOT; $v=Get-ChildItem -LiteralPath $root -Directory -ErrorAction Stop | Where-Object { $_.Name -match '^\d+\.\d+\.\d+$' } | Sort-Object { [version]$_.Name } -Descending | Select-Object -First 1 -ExpandProperty Name; if ($v) { $v }" ^
    > "%FOODFLOW_MSVC_FILE%" 2>nul

if exist "%FOODFLOW_MSVC_FILE%" set /p FOODFLOW_MSVC_VERSION=<"%FOODFLOW_MSVC_FILE%"

del /Q "%FOODFLOW_MSVC_FILE%" >nul 2>&1

if defined FOODFLOW_MSVC_VERSION goto MSVC_VERSION_FOUND

echo.
echo ========================================
echo ERROR: Toolset MSVC no encontrado
echo ========================================
echo.
echo Directorio revisado:
echo.
echo %FOODFLOW_MSVC_ROOT%
echo.
exit /b 1


:MSVC_VERSION_FOUND


REM ============================================================
REM 5. VERIFICAR CL.EXE X64
REM ============================================================

set "FOODFLOW_CL=%FOODFLOW_MSVC_ROOT%\%FOODFLOW_MSVC_VERSION%\bin\Hostx64\x64\cl.exe"

if exist "%FOODFLOW_CL%" goto CL_FOUND

echo.
echo ========================================
echo ERROR: cl.exe x64 no encontrado
echo ========================================
echo.
echo Toolset:
echo %FOODFLOW_MSVC_VERSION%
echo.
echo Ruta esperada:
echo.
echo %FOODFLOW_CL%
echo.
exit /b 1


:CL_FOUND


REM ============================================================
REM 6. LIMPIAR CONFIGURACIONES HEREDADAS
REM ============================================================

set "VCToolsVersion="
set "VSCMD_ARG_VCVARS_VER="


REM ============================================================
REM 7. INICIALIZAR TOOLSET EXPLICITAMENTE
REM ============================================================

set "FOODFLOW_VCVARS_LOG=%TEMP%\foodflow_vcvars_%RANDOM%_%RANDOM%.log"

call "%FOODFLOW_VCVARS64%" ^
    -vcvars_ver=%FOODFLOW_MSVC_VERSION% ^
    > "%FOODFLOW_VCVARS_LOG%" 2>&1

if errorlevel 1 goto VCVARS_ERROR

if exist "%FOODFLOW_VCVARS_LOG%" del /Q "%FOODFLOW_VCVARS_LOG%" >nul 2>&1


REM ============================================================
REM 8. VERIFICACION FINAL
REM ============================================================

if exist "%FOODFLOW_CL%" goto MSVC_READY

echo.
echo ========================================
echo ERROR: MSVC no quedo disponible
echo ========================================
echo.
exit /b 1


:MSVC_READY

exit /b 0


:VCVARS_ERROR

echo.
echo ========================================
echo ERROR: No fue posible inicializar MSVC
echo ========================================
echo.
echo Visual Studio:
echo %FOODFLOW_VS_PATH%
echo.
echo Toolset:
echo %FOODFLOW_MSVC_VERSION%
echo.
echo Detalle:
echo.

if exist "%FOODFLOW_VCVARS_LOG%" type "%FOODFLOW_VCVARS_LOG%"

if exist "%FOODFLOW_VCVARS_LOG%" del /Q "%FOODFLOW_VCVARS_LOG%" >nul 2>&1

exit /b 1