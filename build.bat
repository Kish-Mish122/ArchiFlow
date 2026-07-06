:: build.bat - Исправленная версия
@echo off
chcp 65001 >nul
echo ========================================
echo    Сборка ArchiFlow
echo ========================================
echo.

set MSYS2_PATH=C:\msys64
if not exist "%MSYS2_PATH%" set MSYS2_PATH=C:\msys2

mkdir bin\ArchiFlow 2>nul

:: Создаем скрипт (каждая строка отдельно, без echo)
(
echo #!/bin/bash
echo export PATH=/mingw64/bin:$PATH
echo cd "%CD%"
echo g++ -std=c++17 -O2 -mwindows -s -DUNICODE -D_UNICODE -o bin/ArchiFlow/ArchiFlow.exe ArchiFlow_Viewer.cpp -larchive -lcomctl32 -lcomdlg32 -lshell32 -luxtheme -lgdi32 -luser32 -lkernel32 -ladvapi32 -lole32
echo if [ $? -eq 0 ]^; then
echo   cp /mingw64/bin/libarchive-*.dll bin/ArchiFlow/
echo   cp /mingw64/bin/libbz2-*.dll bin/ArchiFlow/
echo   cp /mingw64/bin/libz*.dll bin/ArchiFlow/
echo   cp /mingw64/bin/liblzma-*.dll bin/ArchiFlow/
echo   cp /mingw64/bin/libzstd-*.dll bin/ArchiFlow/
echo   cp /mingw64/bin/libcrypto-*.dll bin/ArchiFlow/
echo   cp /mingw64/bin/libssl-*.dll bin/ArchiFlow/
echo   cp /mingw64/bin/libiconv-*.dll bin/ArchiFlow/
echo   cp /mingw64/bin/libxml2-*.dll bin/ArchiFlow/
echo   cp /mingw64/bin/libgcc_s_seh-*.dll bin/ArchiFlow/
echo   cp /mingw64/bin/libwinpthread-*.dll bin/ArchiFlow/
echo   cp /mingw64/bin/libstdc++-*.dll bin/ArchiFlow/
echo   echo "SUCCESS"
echo else
echo   echo "FAILED"
echo fi
echo read -p "Press Enter..." dummy
) > "%TEMP%\build_archiflow.sh"

start "Build" /wait "%MSYS2_PATH%\usr\bin\mintty.exe" -t "Build" -e /usr/bin/bash --login "%TEMP%\build_archiflow.sh"

if exist "bin\ArchiFlow\ArchiFlow.exe" (
    echo Готово! bin\ArchiFlow\ArchiFlow.exe
) else (
    echo Ошибка. Запустите вручную в MINGW64:
    echo g++ -std=c++17 -O2 -mwindows -s -DUNICODE -D_UNICODE -o ArchiFlow.exe ArchiFlow_Viewer.cpp -larchive -lcomctl32 -lcomdlg32 -lshell32 -luxtheme -lgdi32 -luser32 -lkernel32 -ladvapi32 -lole32
)
pause