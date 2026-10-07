@echo off
rem Chapter 14, Option 2, Stage 4: the 54 measured runs behind the chapter's
rem two result tables, unattended. Every run is the chapter's launch
rem command (Listing 14.8) with -BenchArm, -BenchN and -BenchI set for one cell.
rem
rem UEBIN is the folder that holds UnrealEditor.exe; it must end in
rem \Engine\Binaries\Win64. Start this file from a Command Prompt in
rem companion\HAPBook\ after typing the two "set" lines of Listing 14.9; the
rem script then uses that UEBIN. If UEBIN is not set, the value below (the Epic
rem Games Launcher default for Unreal Engine 5.8) is used.
rem
rem The project file and the CSV folder are found from this file's own place
rem (companion\HAPBook\Tools\). Move earlier CSV files out of
rem Saved\Profiling\CSV first; the reducer counts every file there.
rem
rem Order: rep -> cell -> arm. Rep 1 of all 18 cells runs first, then rep 2,
rem then rep 3; the three arms of a cell (BP = Pure Blueprint, C++ = Pure C++,
rem Hybrid) run back to back, so slow drift of
rem the machine (heat, power plan) spreads over all arms of a rep.
rem Each run: standalone -game on L_Ch14; the Spawner warms up 10 s, captures
rem 60 s with the CSV Profiler, and the engine exits after writing the CSV
rem (-ExitAfterCsvProfiling). A run that has not exited after TIMEOUT_S seconds
rem is closed and marked FAILED; a run that exits without a new CSV is marked
rem FAILED too. The line after each FAILED line in progress.log is that run's
rem complete command: paste it into the same Command Prompt to re-run the cell.
rem Progress goes to this window and to progress.log in the CSV folder.
rem On the author's laptop the 54 runs took about 80 minutes (one rep of 18
rem runs, about 27 minutes): 20 minutes in, rep 1 is still running.
rem Do not touch mouse or keyboard while it runs.

setlocal EnableDelayedExpansion
if not defined UEBIN set "UEBIN=C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64"
set "PROJ=%~dp0..\HAPBook.uproject"
set "CSVDIR=%~dp0..\Saved\Profiling\CSV"
set "LOG=%CSVDIR%\progress.log"
set "ARM_BP=/Game/Blueprints/BP_Arm_PureBP.BP_Arm_PureBP_C"
set "ARM_CPP=/Script/HAPBook.BenchmarkActorCpp"
set "ARM_HYB=/Game/Blueprints/BP_Arm_Hybrid.BP_Arm_Hybrid_C"
set TIMEOUT_S=180
set TOTAL=54

if not exist "%UEBIN%\UnrealEditor.exe" (
  echo UnrealEditor.exe not found in "%UEBIN%".
  echo Set UEBIN in this window or edit the default at the top of this file;
  echo it must end in \Engine\Binaries\Win64.
  pause
  exit /b 1
)
if not exist "%PROJ%" (
  echo HAPBook.uproject not found at "%PROJ%".
  echo Keep this file in companion\HAPBook\Tools\.
  pause
  exit /b 1
)
tasklist /FI "IMAGENAME eq UnrealEditor.exe" 2>nul | find /I "UnrealEditor.exe" >nul
if not errorlevel 1 (
  echo An UnrealEditor.exe process is running. Close every Unreal Editor window first.
  pause
  exit /b 1
)
if not exist "%CSVDIR%" mkdir "%CSVDIR%"

call :now T0
set "T0HHMM=%time:~0,5%"
set K=0
echo [%date% %time:~0,8%] start: %TOTAL% runs, order rep -^> cell -^> arm, timeout %TIMEOUT_S% s per run>> "%LOG%"
echo Started %T0HHMM%. Progress is also written to "%LOG%".

for /L %%R in (1,1,3) do (
  for %%P in (25:1000 50:1000 100:1000 200:1000 100:100 100:5000) do (
    for /f "tokens=1,2 delims=:" %%N in ("%%P") do (
      for %%A in (BP C++ Hybrid) do (
        set /a K+=1
        call :runone %%R %%A %%N %%O
      )
    )
  )
)
call :now T1
set /a EL=T1-T0
if %EL% LSS 0 set /a EL+=86400
set /a ELM=EL/60
echo [%date% %time:~0,8%] done: %TOTAL% runs in %ELM% min>> "%LOG%"
echo Done: %TOTAL% runs in %ELM% min. Check progress.log for FAILED lines.
endlocal
exit /b 0

rem ---------------------------------------------------------------- one run
:runone
set "R=%~1"
set "A=%~2"
set "N=%~3"
set "I=%~4"
if "%A%"=="BP" set "ARMPATH=%ARM_BP%"
if "%A%"=="C++" set "ARMPATH=%ARM_CPP%"
if "%A%"=="Hybrid" set "ARMPATH=%ARM_HYB%"
call :now TS
set /a EL=TS-T0
if %EL% LSS 0 set /a EL+=86400
set /a ELM=EL/60
set "ETATXT=unknown"
if %K% GTR 1 (
  set /a ETA=EL*^(TOTAL-K+1^)/^(K-1^)/60
  set "ETATXT=!ETA! min"
)
set "HHMM=%time:~0,5%"
set "LINE=cell %K%/%TOTAL%, rep %R%, %A% N=%N% I=%I%, started %HHMM%, elapsed %ELM% min, ETA %ETATXT%"
echo %LINE%
set RUNCMD="%UEBIN%\UnrealEditor.exe" "%PROJ%" /Game/Maps/L_Ch14 -game -nosplash -windowed -ResX=1280 -ResY=720 -ExecCmds="viewmode unlit" -BenchArm=%ARMPATH% -BenchN=%N% -BenchI=%I% -BenchAutoCapture -ExitAfterCsvProfiling
call :countcsv BEFORE
start "" %RUNCMD%
set W=0
:wait
timeout /t 5 /nobreak >nul
set /a W+=5
tasklist /FI "IMAGENAME eq UnrealEditor.exe" 2>nul | find /I "UnrealEditor.exe" >nul
if errorlevel 1 goto exited
if %W% GEQ %TIMEOUT_S% goto killed
goto wait
:killed
taskkill /IM UnrealEditor.exe /F >nul 2>&1
set "STATUS=FAILED (no exit after %TIMEOUT_S% s)"
goto report
:exited
call :countcsv AFTER
set "STATUS=ok"
if %AFTER% LEQ %BEFORE% set "STATUS=FAILED (exited without a new CSV)"
:report
echo   -^> %STATUS%, %W% s
echo [%date% %time:~0,8%] %LINE% -^> %STATUS%, %W% s>> "%LOG%"
if "%STATUS%"=="ok" goto next
echo   re-run: copy the line after this FAILED line in progress.log
>> "%LOG%" echo %RUNCMD%
:next
timeout /t 5 /nobreak >nul
exit /b 0

rem ---------------------------------------------------------------- helpers
:now
set "t=%time: =0%"
set /a "%1=((1%t:~0,2%-100)*3600)+((1%t:~3,2%-100)*60)+(1%t:~6,2%-100)"
exit /b 0

:countcsv
set /a "%1=0"
for /f %%c in ('dir /b "%CSVDIR%\Bench_*.csv" 2^>nul ^| find /c /v ""') do set /a "%1=%%c"
exit /b 0
