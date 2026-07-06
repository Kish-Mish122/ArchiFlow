:: prepare_installer.bat - Подготовка файлов для Inno Setup
@echo off
chcp 65001 >nul
echo ========================================
echo    Подготовка установщика ArchiFlow
echo ========================================
echo.

:: Папка с исходными файлами для установщика
set STAGING=staging
set OUTPUT=Output

:: Очистка
echo Очистка предыдущей сборки...
rmdir /S /Q "%STAGING%" 2>nul
rmdir /S /Q "%OUTPUT%" 2>nul

:: Создание структуры папок
echo Создание структуры папок...
mkdir "%STAGING%"
mkdir "%STAGING%\bin"
mkdir "%STAGING%\resources"
mkdir "%STAGING%\lang\ru-RU"
mkdir "%STAGING%\lang\en-US"
mkdir "%STAGING%\data"
mkdir "%STAGING%\plugins"
mkdir "%STAGING%\redist"
mkdir "%OUTPUT%"

:: Копирование exe и dll
echo Копирование программы...
copy "bin\ArchiFlow\ArchiFlow.exe" "%STAGING%\bin\" >nul
xcopy "bin\ArchiFlow\*.dll" "%STAGING%\bin\" /Y /Q >nul

:: Копирование ресурсов (если есть)
if exist "resources\*" (
    xcopy "resources\*" "%STAGING%\resources\" /E /I /Y /Q >nul
) else (
    echo. > "%STAGING%\resources\placeholder"
)

:: Копирование языковых файлов
if exist "lang\ru-RU\*" (
    xcopy "lang\ru-RU\*" "%STAGING%\lang\ru-RU\" /E /I /Y /Q >nul
) else (
    echo. > "%STAGING%\lang\ru-RU\placeholder"
)

if exist "lang\en-US\*" (
    xcopy "lang\en-US\*" "%STAGING%\lang\en-US\" /E /I /Y /Q >nul
) else (
    echo. > "%STAGING%\lang\en-US\placeholder"
)

:: Создание license.txt если нет
if not exist "license.txt" (
    echo MIT License > "%STAGING%\license.txt"
    echo. >> "%STAGING%\license.txt"
    echo Copyright (c) 2024 K1sh-M1sh >> "%STAGING%\license.txt"
    echo. >> "%STAGING%\license.txt"
    echo Permission is hereby granted, free of charge, to any person obtaining a copy >> "%STAGING%\license.txt"
    echo of this software and associated documentation files, to deal in the Software >> "%STAGING%\license.txt"
    echo without restriction, including without limitation the rights to use, copy, >> "%STAGING%\license.txt"
    echo modify, merge, publish, distribute, sublicense, and/or sell copies of the >> "%STAGING%\license.txt"
    echo Software, and to permit persons to whom the Software is furnished to do so. >> "%STAGING%\license.txt"
)

:: Создание readme.txt если нет
if not exist "readme.txt" (
    echo ArchiFlow - Легкий архиватор > "%STAGING%\readme.txt"
    echo. >> "%STAGING%\readme.txt"
    echo Поддерживаемые форматы: >> "%STAGING%\readme.txt"
    echo - ZIP (основной) >> "%STAGING%\readme.txt"
    echo - 7Z, RAR, TAR, GZIP, BZIP2 (просмотр) >> "%STAGING%\readme.txt"
)

:: Создание changelog.txt
echo v1.0.0 - Первый релиз > "%STAGING%\changelog.txt"
echo - Создание ZIP архивов >> "%STAGING%\changelog.txt"
echo - Просмотр содержимого архивов >> "%STAGING%\changelog.txt"
echo - Извлечение файлов >> "%STAGING%\changelog.txt"
echo - Интерфейс на русском и английском >> "%STAGING%\changelog.txt"

:: Копирование данных
echo. > "%STAGING%\data\placeholder"
echo. > "%STAGING%\plugins\placeholder"

:: Копирование redist (vc_redist) если есть
if exist "redist\vc_redist.x86.exe" (
    copy "redist\vc_redist.x86.exe" "%STAGING%\redist\" >nul
)

:: Создание иконки если нет ресурсов
if not exist "%STAGING%\resources\archiflow.ico" (
    echo Создание иконки-заглушки...
    echo. > "%STAGING%\resources\archiflow.ico"
)

:: Копирование скрипта Inno Setup в staging
copy "ArchiFlow_Setup.iss" "%STAGING%\" >nul

echo.
echo ========================================
echo    Подготовка завершена!
echo ========================================
echo.
echo Структура папки staging:
dir "%STAGING%" /B /S
echo.
echo Теперь:
echo 1. Откройте "%STAGING%\ArchiFlow_Setup.iss" в Inno Setup Compiler
echo 2. Нажмите Build -^> Compile
echo 3. Установщик будет в папке Output
echo.
pause