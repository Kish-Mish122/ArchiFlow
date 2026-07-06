; ArchiFlow_Setup.iss - Inno Setup Installation Script
; ArchiFlow - Lightweight Archiver
; Supports multiple languages: Russian and English

#define MyAppName "ArchiFlow"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "K1sh-M1sh"
#define MyAppURL "https://github.com/Kish-Mish122/archiflow"
#define MyAppExeName "ArchiFlow.exe"
#define MyAppAssocName "ArchiFlow.Archive"

[Setup]
AppId={{B8F4A3D2-C5E6-4F7A-8B9C-0D1E2F3A4B5C}}
AppName={#MyAppName}
AppVersion={#MyAppVersion}; ArchiFlow_Setup.iss - Inno Setup Installation Script
#define MyAppName "ArchiFlow"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "K1sh-M1sh"
#define MyAppURL "https://github.com/Kish-Mish122/archiflow"
#define MyAppExeName "ArchiFlow.exe"
#define MyAppAssocName "ArchiFlow.Archive"

[Setup]
AppId={{B8F4A3D2-C5E6-4F7A-8B9C-0D1E2F3A4B5C}}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
LicenseFile=license.txt
OutputDir=..\Output
OutputBaseFilename=ArchiFlow_Setup_v1.0.0
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
UninstallDisplayIcon={app}\{#MyAppExeName}
UninstallDisplayName={#MyAppName}
VersionInfoVersion=1.0.0.0
VersionInfoCompany={#MyAppPublisher}
VersionInfoDescription=ArchiFlow - Lightweight Archiver
VersionInfoCopyright=MIT License
MinVersion=6.1.7601
PrivilegesRequired=lowest

[Languages]
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[CustomMessages]
russian.AppName=ArchiFlow
russian.CreateDesktopIcon=Создать ярлык на рабочем столе
russian.AddContextMenu=Добавить в контекстное меню (ПКМ)
russian.AssociateFiles=Ассоциировать форматы архивов с ArchiFlow
russian.LaunchProgram=Запустить ArchiFlow
english.AppName=ArchiFlow
english.CreateDesktopIcon=Create desktop icon
english.AddContextMenu=Add to context menu (Right Click)
english.AssociateFiles=Associate archive formats with ArchiFlow
english.LaunchProgram=Launch ArchiFlow

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "Дополнительные ярлыки:"
Name: "contextmenu"; Description: "{cm:AddContextMenu}"; GroupDescription: "Интеграция в систему:"
Name: "associations"; Description: "{cm:AssociateFiles}"; GroupDescription: "Интеграция в систему:"

[Files]
; Основные файлы программы
Source: "bin\ArchiFlow.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "bin\*.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "resources\*"; DestDir: "{app}\resources"; Flags: ignoreversion recursesubdirs
Source: "data\*"; DestDir: "{app}\data"; Flags: ignoreversion recursesubdirs
Source: "plugins\*"; DestDir: "{app}\plugins"; Flags: ignoreversion recursesubdirs
Source: "license.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "readme.txt"; DestDir: "{app}"; Flags: ignoreversion isreadme
Source: "changelog.txt"; DestDir: "{app}"; Flags: ignoreversion

; Языковые файлы
Source: "lang\ru-RU\*"; DestDir: "{app}\lang\ru-RU"; Flags: ignoreversion recursesubdirs
Source: "lang\en-US\*"; DestDir: "{app}\lang\en-US"; Flags: ignoreversion recursesubdirs

; Visual C++ Redistributable (если есть)
Source: "redist\vc_redist.x86.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall; Check: IsVCRedistNeeded

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}"
Name: "{group}\Удалить ArchiFlow"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Registry]
; Регистрация приложения
Root: HKCU; Subkey: "Software\{#MyAppPublisher}\{#MyAppName}"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\{#MyAppPublisher}\{#MyAppName}"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}"
Root: HKCU; Subkey: "Software\{#MyAppPublisher}\{#MyAppName}"; ValueType: string; ValueName: "Version"; ValueData: "{#MyAppVersion}"

; Контекстное меню для всех файлов
Root: HKCR; Subkey: "*\shell\ArchiFlow.AddToArchive"; ValueType: string; ValueName: "MUIVerb"; ValueData: "Добавить в архив"; Tasks: contextmenu; Flags: uninsdeletekey
Root: HKCR; Subkey: "*\shell\ArchiFlow.AddToArchive"; ValueType: string; ValueName: "Icon"; ValueData: "{app}\{#MyAppExeName}"; Tasks: contextmenu
Root: HKCR; Subkey: "*\shell\ArchiFlow.AddToArchive\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" -a ""%1"""; Tasks: contextmenu

; Контекстное меню для папок
Root: HKCR; Subkey: "Directory\shell\ArchiFlow.AddToArchive"; ValueType: string; ValueName: "MUIVerb"; ValueData: "Добавить в архив"; Tasks: contextmenu; Flags: uninsdeletekey
Root: HKCR; Subkey: "Directory\shell\ArchiFlow.AddToArchive"; ValueType: string; ValueName: "Icon"; ValueData: "{app}\{#MyAppExeName}"; Tasks: contextmenu
Root: HKCR; Subkey: "Directory\shell\ArchiFlow.AddToArchive\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" -a ""%1"""; Tasks: contextmenu

; Ассоциации файлов
Root: HKCR; Subkey: ".zip"; ValueType: string; ValueName: ""; ValueData: "{#MyAppAssocName}"; Flags: uninsdeletevalue; Tasks: associations
Root: HKCR; Subkey: ".7z"; ValueType: string; ValueName: ""; ValueData: "{#MyAppAssocName}"; Flags: uninsdeletevalue; Tasks: associations
Root: HKCR; Subkey: ".rar"; ValueType: string; ValueName: ""; ValueData: "{#MyAppAssocName}"; Flags: uninsdeletevalue; Tasks: associations

; ProgID для ассоциаций
Root: HKCR; Subkey: "{#MyAppAssocName}"; ValueType: string; ValueName: ""; ValueData: "ArchiFlow Archive"; Flags: uninsdeletekey; Tasks: associations
Root: HKCR; Subkey: "{#MyAppAssocName}\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Tasks: associations
Root: HKCR; Subkey: "{#MyAppAssocName}\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Tasks: associations
Root: HKCR; Subkey: "{#MyAppAssocName}\shell\extract"; ValueType: string; ValueName: "MUIVerb"; ValueData: "Извлечь в..."; Tasks: associations
Root: HKCR; Subkey: "{#MyAppAssocName}\shell\extract\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Tasks: associations

[Run]
; Установка VC++ Redist
Filename: "{tmp}\vc_redist.x86.exe"; Parameters: "/quiet /norestart"; Check: IsVCRedistNeeded; StatusMsg: "Установка Visual C++ Redistributable..."

; Запуск программы
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram}"; Flags: nowait postinstall skipifsilent

[Code]
function IsVCRedistNeeded: Boolean;
begin
  Result := not (RegKeyExists(HKLM, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x86') or
                 RegKeyExists(HKLM, 'SOFTWARE\WOW6432Node\Microsoft\VisualStudio\14.0\VC\Runtimes\x86'));
end;

function InitializeSetup: Boolean;
begin
  Result := True;
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    if not DirExists(ExpandConstant('{userappdata}\ArchiFlow')) then
      CreateDir(ExpandConstant('{userappdata}\ArchiFlow'));
    SaveStringToFile(ExpandConstant('{userappdata}\ArchiFlow\language.ini'),
      'Language=' + ActiveLanguage, False);
  end;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  if CurUninstallStep = usPostUninstall then
  begin
    DelTree(ExpandConstant('{userappdata}\ArchiFlow'), True, True, True);
  end;
end;
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
LicenseFile=license.txt
OutputDir=.\Output
OutputBaseFilename=ArchiFlow_Setup
SetupIconFile=resources\archiflow.ico
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
WizardImageFile=resources\wizard_image.bmp
WizardSmallImageFile=resources\wizard_small.bmp
UninstallDisplayIcon={app}\{#MyAppExeName}
UninstallDisplayName={#MyAppName}
VersionInfoVersion=1.0.0.0
VersionInfoCompany={#MyAppPublisher}
VersionInfoDescription=ArchiFlow - Lightweight Archiver
VersionInfoCopyright=MIT
MinVersion=6.1.7601

[Languages]
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[CustomMessages]
russian.AppName=ArchiFlow
russian.AppDescription=Легкий архиватор
russian.CreateDesktopIcon=Создать ярлык на рабочем столе
russian.CreateQuickLaunchIcon=Создать ярлык в панели быстрого запуска
russian.AddContextMenu=Добавить в контекстное меню
russian.AssociateFiles=Ассоциировать форматы архивов
russian.LaunchProgram=Запустить ArchiFlow
russian.ViewReadme=Просмотреть README файл
russian.InstallVCRedist=Установка Visual C++ Redistributable
russian.DownloadingVCRedist=Загрузка Visual C++ Redistributable...
russian.InstallingVCRedist=Установка Visual C++ Redistributable...

english.AppName=ArchiFlow
english.AppDescription=Lightweight Archiver
english.CreateDesktopIcon=Create desktop icon
english.CreateQuickLaunchIcon=Create quick launch icon
english.AddContextMenu=Add to context menu
english.AssociateFiles=Associate archive formats
english.LaunchProgram=Launch ArchiFlow
english.ViewReadme=View README file
english.InstallVCRedist=Install Visual C++ Redistributable
english.DownloadingVCRedist=Downloading Visual C++ Redistributable...
english.InstallingVCRedist=Installing Visual C++ Redistributable...

[Types]
Name: "full"; Description: "Полная установка"
Name: "custom"; Description: "Выборочная установка"; Flags: iscustom

[Components]
Name: "main"; Description: "Основные файлы программы"; Types: full custom; Flags: fixed
Name: "contextmenu"; Description: "Интеграция в контекстное меню"; Types: full
Name: "associations"; Description: "Ассоциации файлов"; Types: full
Name: "languages"; Description: "Языковые пакеты"; Types: full
Name: "languages\russian"; Description: "Русский язык"; Types: full custom; Flags: fixed
Name: "languages\english"; Description: "English language"; Types: full custom; Flags: fixed

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "Дополнительные ярлыки:"
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "Дополнительные ярлыки:"; Flags: unchecked
Name: "contextmenutask"; Description: "{cm:AddContextMenu}"; GroupDescription: "Интеграция в систему:"
Name: "associatetask"; Description: "{cm:AssociateFiles}"; GroupDescription: "Интеграция в систему:"

[Files]
Source: "bin\Release\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion; Components: main
Source: "bin\Release\*.dll"; DestDir: "{app}"; Flags: ignoreversion; Components: main
Source: "bin\Release\resources\*"; DestDir: "{app}\resources"; Flags: ignoreversion recursesubdirs; Components: main
Source: "bin\Release\data\*"; DestDir: "{app}\data"; Flags: ignoreversion; Components: main
Source: "bin\Release\plugins\*"; DestDir: "{app}\plugins"; Flags: ignoreversion; Components: main
Source: "license.txt"; DestDir: "{app}"; Flags: ignoreversion; Components: main
Source: "readme.txt"; DestDir: "{app}"; Flags: ignoreversion isreadme; Components: main
Source: "changelog.txt"; DestDir: "{app}"; Flags: ignoreversion; Components: main

; Language files
Source: "lang\ru-RU\*"; DestDir: "{app}\lang\ru-RU"; Flags: ignoreversion recursesubdirs; Components: languages\russian
Source: "lang\en-US\*"; DestDir: "{app}\lang\en-US"; Flags: ignoreversion recursesubdirs; Components: languages\english

; Visual C++ Redistributable
Source: "redist\vc_redist.x86.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall; Check: IsVCRedistNeeded

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}"
Name: "{group}\Удалить ArchiFlow"; Filename: "{uninstallexe}"
Name: "{group}\README"; Filename: "{app}\readme.txt"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon

[Registry]
; Register application
Root: HKCU; Subkey: "Software\{#MyAppPublisher}\{#MyAppName}"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\{#MyAppPublisher}\{#MyAppName}"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}"
Root: HKCU; Subkey: "Software\{#MyAppPublisher}\{#MyAppName}"; ValueType: string; ValueName: "Version"; ValueData: "{#MyAppVersion}"
Root: HKCU; Subkey: "Software\{#MyAppPublisher}\{#MyAppName}"; ValueType: string; ValueName: "Language"; ValueData: "{language}"

; Application registration
Root: HKLM; Subkey: "Software\Microsoft\Windows\CurrentVersion\App Paths\{#MyAppExeName}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\Microsoft\Windows\CurrentVersion\App Paths\{#MyAppExeName}"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName}"
Root: HKLM; Subkey: "Software\Microsoft\Windows\CurrentVersion\App Paths\{#MyAppExeName}"; ValueType: string; ValueName: "Path"; ValueData: "{app}"

; Context menu integration
Root: HKCR; Subkey: "*\shellex\ContextMenuHandlers\{#MyAppName}"; ValueType: string; ValueName: ""; ValueData: "{{B8F4A3D2-C5E6-4F7A-8B9C-0D1E2F3A4B5C}}"; Flags: uninsdeletekey; Tasks: contextmenutask
Root: HKCR; Subkey: "Directory\shellex\ContextMenuHandlers\{#MyAppName}"; ValueType: string; ValueName: ""; ValueData: "{{B8F4A3D2-C5E6-4F7A-8B9C-0D1E2F3A4B5C}}"; Flags: uninsdeletekey; Tasks: contextmenutask
Root: HKCR; Subkey: "Drive\shellex\ContextMenuHandlers\{#MyAppName}"; ValueType: string; ValueName: ""; ValueData: "{{B8F4A3D2-C5E6-4F7A-8B9C-0D1E2F3A4B5C}}"; Flags: uninsdeletekey; Tasks: contextmenutask

; File associations
Root: HKCR; Subkey: ".zip"; ValueType: string; ValueName: ""; ValueData: "{#MyAppAssocName}"; Flags: uninsdeletevalue; Tasks: associatetask
Root: HKCR; Subkey: ".7z"; ValueType: string; ValueName: ""; ValueData: "{#MyAppAssocName}"; Flags: uninsdeletevalue; Tasks: associatetask
Root: HKCR; Subkey: ".rar"; ValueType: string; ValueName: ""; ValueData: "{#MyAppAssocName}"; Flags: uninsdeletevalue; Tasks: associatetask
Root: HKCR; Subkey: ".tar"; ValueType: string; ValueName: ""; ValueData: "{#MyAppAssocName}"; Flags: uninsdeletevalue; Tasks: associatetask
Root: HKCR; Subkey: ".gz"; ValueType: string; ValueName: ""; ValueData: "{#MyAppAssocName}"; Flags: uninsdeletevalue; Tasks: associatetask
Root: HKCR; Subkey: ".bz2"; ValueType: string; ValueName: ""; ValueData: "{#MyAppAssocName}"; Flags: uninsdeletevalue; Tasks: associatetask

; ProgID registration
Root: HKCR; Subkey: "{#MyAppAssocName}"; ValueType: string; ValueName: ""; ValueData: "ArchiFlow Archive"; Flags: uninsdeletekey; Tasks: associatetask
Root: HKCR; Subkey: "{#MyAppAssocName}\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Tasks: associatetask
Root: HKCR; Subkey: "{#MyAppAssocName}\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Tasks: associatetask
Root: HKCR; Subkey: "{#MyAppAssocName}\shell\extract\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" -x ""%1"""; Tasks: associatetask

[Run]
; Install Visual C++ Redistributable if needed
Filename: "{tmp}\vc_redist.x86.exe"; Parameters: "/quiet /norestart"; Check: IsVCRedistNeeded; StatusMsg: "{cm:InstallingVCRedist}"

; Register context menu DLL
Filename: "{sys}\regsvr32.exe"; Parameters: "/s ""{app}\ArchiFlowShell.dll"""; Tasks: contextmenutask; Flags: runhidden

; Launch application
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram}"; Flags: nowait postinstall skipifsilent

[UninstallRun]
; Unregister context menu DLL
Filename: "{sys}\regsvr32.exe"; Parameters: "/u /s ""{app}\ArchiFlowShell.dll"""; Flags: runhidden

[InstallDelete]
; Clean up old versions
Type: filesandordirs; Name: "{app}\data\cache"
Type: filesandordirs; Name: "{app}\temp"

[UninstallDelete]
Type: filesandordirs; Name: "{app}"

[Code]
var
  DownloadPage: TDownloadWizardPage;

function IsVCRedistNeeded: Boolean;
var
  bNeedInstall: Boolean;
begin
  bNeedInstall := False;
  
  // Check if Visual C++ 2015-2022 Redistributable is installed
  if not RegKeyExists(HKLM, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x86') then
  begin
    if not RegKeyExists(HKLM, 'SOFTWARE\WOW6432Node\Microsoft\VisualStudio\14.0\VC\Runtimes\x86') then
    begin
      bNeedInstall := True;
    end;
  end;
  
  Result := bNeedInstall;
end;

function InitializeSetup: Boolean;
begin
  Result := True;
end;

procedure InitializeWizard;
begin
  DownloadPage := CreateDownloadPage(SetupMessage(msgWizardPreparing), SetupMessage(msgPreparingDesc), @OnDownloadProgress);
end;

function NextButtonClick(CurPageID: Integer): Boolean;
begin
  Result := True;
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    // Create necessary directories
    if not DirExists(ExpandConstant('{userappdata}\ArchiFlow')) then
      CreateDir(ExpandConstant('{userappdata}\ArchiFlow'));
    
    // Write configuration
    SaveStringToFile(ExpandConstant('{userappdata}\ArchiFlow\language.ini'), 
      'Language=' + ActiveLanguage, False);
  end;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  if CurUninstallStep = usPostUninstall then
  begin
    // Remove user data
    DelTree(ExpandConstant('{userappdata}\ArchiFlow'), True, True, True);
  end;
end;