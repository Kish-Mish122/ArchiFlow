:: cleanup.bat - Очистка проекта от лишних файлов
@echo off
chcp 65001 >nul
echo ========================================
echo    Очистка проекта ArchiFlow
echo ========================================
echo.

echo Удаление временных файлов...
del /Q *.o 2>nul
del /Q *.obj 2>nul
del /Q *.res 2>nul

echo Удаление старых версий...
del /Q ArchiFlow.exe 2>nul
del /Q ArchiFlow_Full.exe 2>nul
del /Q ArchiFlow_ZIP.exe 2>nul
del /Q ArchiFlow_Working.exe 2>nul
del /Q ArchiFlow_Minimal.exe 2>nul
del /Q test_archive.exe 2>nul

echo Удаление временных файлов C++...
del /Q ArchiFlow_Minimal.cpp 2>nul
del /Q ArchiFlow_Fixed.cpp 2>nul
del /Q ArchiFlow_Working.cpp 2>nul
del /Q ArchiFlow_ZIP.cpp 2>nul
del /Q ArchiFlow_Full.cpp 2>nul
del /Q ArchiFlow_Simple.cpp 2>nul

echo Удаление заглушек...
del /Q RegistryManager.cpp 2>nul
del /Q ShellExtension.cpp 2>nul
del /Q ConfigManager.cpp 2>nul
del /Q ThemeManager.cpp 2>nul
del /Q DragDropHandler.cpp 2>nul
del /Q FileUtils.cpp 2>nul
del /Q ArchiFlowInstaller.cpp 2>nul
del /Q UninstallHelper.cpp 2>nul

echo Удаление папок сборки...
rmdir /S /Q build 2>nul
rmdir /S /Q bin 2>nul
rmdir /S /Q build_cross 2>nul
rmdir /S /Q build_libarchive 2>nul
rmdir /S /Q build_libs 2>nul
rmdir /S /Q build_mingw 2>nul
rmdir /S /Q obj 2>nul
rmdir /S /Q Output 2>nul
rmdir /S /Q package 2>nul
rmdir /S /Q portable 2>nul

echo Удаление временных скриптов...
del /Q fix_msys2_russia.bat 2>nul
del /Q fix_msys2_russia_fixed.bat 2>nul
del /Q fix_msys2_yandex_only.bat 2>nul
del /Q fix_vcpkg_bzip2.bat 2>nul
del /Q install_libarchive_direct.bat 2>nul
del /Q install_libarchive_msys2.bat 2>nul
del /Q install_libarchive_vcpkg.bat 2>nul
del /Q install_with_conan.bat 2>nul
del /Q install_with_msys2.bat 2>nul
del /Q manual_download_russia.bat 2>nul
del /Q manual_msys2_steps.bat 2>nul
del /Q download_libarchive_prebuilt.bat 2>nul
del /Q download_from_russia_mirrors.bat 2>nul
del /Q direct_download_msys2_packages.bat 2>nul
del /Q compile_libarchive_manual.bat 2>nul
del /Q quick_fix_download_prebuilt.bat 2>nul
del /Q setup_dependencies.bat 2>nul
del /Q build_libarchive_from_source.bat 2>nul
del /Q manual_download_bzip2.bat 2>nul
del /Q vcpkg_with_proxy.bat 2>nul
del /Q build_from_local_sources.bat 2>nul
del /Q copy_msys2_to_project.bat 2>nul
del /Q create_all_files.bat 2>nul
del /Q create_fixed_main.bat 2>nul
del /Q create_stubs.bat 2>nul
del /Q ultimate_fix_and_build.bat 2>nul
del /Q build_archiflow_complete.bat 2>nul
del /Q build_correct_mingw.bat 2>nul
del /Q build_final_now.bat 2>nul
del /Q build_minimal.bat 2>nul
del /Q build_simple_now.bat 2>nul
del /Q build_with_msys2.bat 2>nul
del /Q build_with_vs.bat 2>nul
del /Q check_libarchive.bat 2>nul
del /Q open_mingw64_terminal.bat 2>nul
del /Q simple_test_compile.bat 2>nul

echo Удаление временных скриптов sh...
del /Q build_final.sh 2>nul
del /Q build.sh 2>nul

echo Удаление тестовых файлов...
del /Q test.txt 2>nul
del /Q test_archive.c 2>nul
del /Q test.zip 2>nul
del /Q archive.zip 2>nul

echo.
echo ========================================
echo    Очистка завершена!
echo ========================================
echo.
echo Оставлены только нужные файлы:
echo - ArchiFlow_Viewer.cpp (основной исходник)
echo - ArchiFlow_Viewer.exe (готовый exe)
echo - build.bat (скрипт сборки)
echo - setup.bat (скрипт установки зависимостей)
echo.
pause