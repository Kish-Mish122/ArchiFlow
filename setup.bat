:: setup.bat - Установка всех зависимостей для ArchiFlow
@echo off
chcp 65001 >nul
echo ========================================
echo    Установка зависимостей ArchiFlow
echo ========================================
echo.

set MSYS2_PATH=C:\msys64

:: Проверка MSYS2
if not exist "%MSYS2_PATH%\usr\bin\bash.exe" (
    if not exist "C:\msys2\usr\bin\bash.exe" (
        echo MSYS2 не найден.
        echo.
        echo Скачайте MSYS2 с https://www.msys2.org/
        echo Или с зеркала: https://mirror.yandex.ru/mirrors/msys2/
        echo.
        echo Установите в C:\msys64 и запустите этот скрипт снова.
        pause
        exit /b 1
    )
    set MSYS2_PATH=C:\msys2
)

echo MSYS2 найден: %MSYS2_PATH%
echo.

:: Закрываем процессы MSYS2
taskkill /F /IM bash.exe 2>nul
taskkill /F /IM pacman.exe 2>nul
timeout /t 2 /nobreak >nul

:: Удаляем блокировку
if exist "%MSYS2_PATH%\var\lib\pacman\db.lck" del /F /Q "%MSYS2_PATH%\var\lib\pacman\db.lck"

:: Создаем скрипт установки
(
echo #!/bin/bash
echo echo "Обновление зеркал..."
echo echo 'Server = https://mirror.yandex.ru/mirrors/msys2/mingw/x86_64/' ^> /etc/pacman.d/mirrorlist.mingw64
echo echo 'Server = https://mirror.yandex.ru/mirrors/msys2/mingw/i686/' ^> /etc/pacman.d/mirrorlist.mingw32
echo echo 'Server = https://mirror.yandex.ru/mirrors/msys2/msys/x86_64/' ^> /etc/pacman.d/mirrorlist.msys
echo.
echo echo "Обновление базы пакетов..."
echo pacman -Syy --noconfirm --disable-download-timeout
echo.
echo echo "Установка компилятора и библиотек..."
echo pacman -S --noconfirm --disable-download-timeout ^
echo     mingw-w64-x86_64-gcc ^
echo     mingw-w64-x86_64-libarchive ^
echo     mingw-w64-x86_64-zlib ^
echo     mingw-w64-x86_64-bzip2 ^
echo     mingw-w64-x86_64-xz ^
echo     mingw-w64-x86_64-zstd ^
echo     mingw-w64-x86_64-openssl ^
echo     mingw-w64-x86_64-libiconv ^
echo     mingw-w64-x86_64-libxml2
echo.
echo echo ""
echo echo "========================================="
echo echo "  Установка завершена!"
echo echo "========================================="
echo echo ""
echo sleep 3
) > "%TEMP%\setup_archiflow.sh"

echo Запуск установки пакетов...
echo Это может занять несколько минут...
echo.

"%MSYS2_PATH%\usr\bin\mintty.exe" -t "Установка ArchiFlow" -e /usr/bin/bash --login "%TEMP%\setup_archiflow.sh"

echo.
echo ========================================
echo    Установка завершена!
echo ========================================
echo.
echo Теперь запустите build.bat для сборки.
pause