@echo off
REM package.bat - Package application for distribution

echo ========================================
echo    ArchiFlow Package Script
echo ========================================
echo.

set VERSION=1.0.0
set PACKAGE_DIR=package
set OUTPUT_DIR=Output

echo Creating package directory...
if exist %PACKAGE_DIR% rmdir /s /q %PACKAGE_DIR%
mkdir %PACKAGE_DIR%
mkdir %PACKAGE_DIR%\bin
mkdir %PACKAGE_DIR%\resources
mkdir %PACKAGE_DIR%\lang
mkdir %PACKAGE_DIR%\data
mkdir %PACKAGE_DIR%\plugins

echo Copying files...
copy bin\Release\ArchiFlow.exe %PACKAGE_DIR%\bin\
copy bin\Release\*.dll %PACKAGE_DIR%\bin\
xcopy resources\* %PACKAGE_DIR%\resources\ /E /I
xcopy lang\* %PACKAGE_DIR%\lang\ /E /I
copy license.txt %PACKAGE_DIR%\
copy readme.txt %PACKAGE_DIR%\
copy changelog.txt %PACKAGE_DIR%\

echo Compiling installer...
"C:\Program Files (x86)\Inno Setup 6\ISCC.exe" ArchiFlow_Setup.iss

echo.
echo ========================================
echo    Package created successfully!
echo    Output: %OUTPUT_DIR%\ArchiFlow_Setup_v%VERSION%.exe
echo ========================================

pause