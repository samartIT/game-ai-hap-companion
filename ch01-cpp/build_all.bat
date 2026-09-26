@echo off
REM คอมไพล์และรันตัวอย่างทั้งหมดของบทที่ 1 ด้วย MSVC
REM ต้องเปิดจาก "x64 Native Tools Command Prompt for VS" (มากับ Visual Studio Build Tools)
cd /d "%~dp0"
if not exist build mkdir build
for %%f in (01_hello 02_syntax 03_pointers 04_struct 05_class 06_inheritance 07_polymorphism) do (
  echo === %%f ===
  cl /nologo /EHsc /W4 /utf-8 /Fe:build\%%f.exe %%f.cpp >nul
  build\%%f.exe
  echo.
)
echo === 08_header_source ===
cl /nologo /EHsc /W4 /utf-8 /Fe:build\08_header_source.exe 08_header_source\main.cpp 08_header_source\GameCharacter.cpp >nul
build\08_header_source.exe
