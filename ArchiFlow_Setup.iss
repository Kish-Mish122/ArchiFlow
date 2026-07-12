; ArchiFlow_Setup.iss - Inno Setup Installation Script
; ArchiFlow - Lightweight Archiver
; Supports multiple languages: Russian and English

#define MyAppName "ArchiFlow"
#define MyAppVersion "1.2.0"
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
OutputDir=.\Output
OutputBaseFilename=ArchiFlow_Setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
UninstallDisplayIcon={app}\{#MyAppExeName}
UninstallDisplayName={#MyAppName}
VersionInfoVersion=1.2.0.0
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
russian.AddContextMenu=Добавить в контекстное меню (ПКМ)
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
english.AddContextMenu=Add to context menu (Right Click)
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
; Основные файлы программы
Source: "bin\ArchiFlow\ArchiFlow.exe"; DestDir: "{app}"; Flags: ignoreversion; Components: main
Source: "bin\ArchiFlow\*.dll"; DestDir: "{app}"; Flags: ignoreversion; Components: main
Source: "license.txt"; DestDir: "{app}"; Flags: ignoreversion; Components: main

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}"
Name: "{group}\Удалить ArchiFlow"; Filename: "{uninstallexe}"
Name: "{group}\README"; Filename: "{app}\readme.txt"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon

[Registry]
; Регистрация приложения
Root: HKCU; Subkey: "Software\{#MyAppPublisher}\{#MyAppName}"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\{#MyAppPublisher}\{#MyAppName}"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}"
Root: HKCU; Subkey: "Software\{#MyAppPublisher}\{#MyAppName}"; ValueType: string; ValueName: "Version"; ValueData: "{#MyAppVersion}"
Root: HKCU; Subkey: "Software\{#MyAppPublisher}\{#MyAppName}"; ValueType: string; ValueName: "Language"; ValueData: "{language}"

; Контекстное меню для всех файлов (создание архива)
Root: HKCR; Subkey: "*\shell\ArchiFlow.AddToArchive"; ValueType: string; ValueName: "MUIVerb"; ValueData: "Добавить в архив"; Tasks: contextmenutask; Flags: uninsdeletekey
Root: HKCR; Subkey: "*\shell\ArchiFlow.AddToArchive"; ValueType: string; ValueName: "Icon"; ValueData: "{app}\{#MyAppExeName}"; Tasks: contextmenutask
Root: HKCR; Subkey: "*\shell\ArchiFlow.AddToArchive\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"""; Tasks: contextmenutask

; Контекстное меню для папок (создание архива)
Root: HKCR; Subkey: "Directory\Background\shell\ArchiFlow.CreateArchive"; ValueType: string; ValueName: "MUIVerb"; ValueData: "Создать архив"; Tasks: contextmenutask; Flags: uninsdeletekey
Root: HKCR; Subkey: "Directory\Background\shell\ArchiFlow.CreateArchive"; ValueType: string; ValueName: "Icon"; ValueData: "{app}\{#MyAppExeName}"; Tasks: contextmenutask
Root: HKCR; Subkey: "Directory\Background\shell\ArchiFlow.CreateArchive\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"""; Tasks: contextmenutask

; Контекстное меню для папок (открытие папки)
Root: HKCR; Subkey: "Directory\shell\ArchiFlow.OpenFolder"; ValueType: string; ValueName: "MUIVerb"; ValueData: "Открыть папку в ArchiFlow"; Tasks: contextmenutask; Flags: uninsdeletekey
Root: HKCR; Subkey: "Directory\shell\ArchiFlow.OpenFolder"; ValueType: string; ValueName: "Icon"; ValueData: "{app}\{#MyAppExeName}"; Tasks: contextmenutask
Root: HKCR; Subkey: "Directory\shell\ArchiFlow.OpenFolder\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"""; Tasks: contextmenutask

; Ассоциации файлов
Root: HKCR; Subkey: ".zip"; ValueType: string; ValueName: ""; ValueData: "{#MyAppAssocName}"; Flags: uninsdeletevalue; Tasks: associatetask
Root: HKCR; Subkey: ".7z"; ValueType: string; ValueName: ""; ValueData: "{#MyAppAssocName}"; Flags: uninsdeletevalue; Tasks: associatetask
Root: HKCR; Subkey: ".rar"; ValueType: string; ValueName: ""; ValueData: "{#MyAppAssocName}"; Flags: uninsdeletevalue; Tasks: associatetask
Root: HKCR; Subkey: ".tar"; ValueType: string; ValueName: ""; ValueData: "{#MyAppAssocName}"; Flags: uninsdeletevalue; Tasks: associatetask
Root: HKCR; Subkey: ".gz"; ValueType: string; ValueName: ""; ValueData: "{#MyAppAssocName}"; Flags: uninsdeletevalue; Tasks: associatetask

; ProgID для ассоциаций
Root: HKCR; Subkey: "{#MyAppAssocName}"; ValueType: string; ValueName: ""; ValueData: "ArchiFlow Archive"; Flags: uninsdeletekey; Tasks: associatetask
Root: HKCR; Subkey: "{#MyAppAssocName}\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Tasks: associatetask
Root: HKCR; Subkey: "{#MyAppAssocName}\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Tasks: associatetask

[Run]
; Установка VC++ Redist
Filename: "{tmp}\vc_redist.x86.exe"; Parameters: "/quiet /norestart"; Check: IsVCRedistNeeded; StatusMsg: "{cm:InstallingVCRedist}"

; Запуск программы
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram}"; Flags: nowait postinstall skipifsilent

[UninstallRun]
; Удаление данных пользователя
Filename: "{sys}\regsvr32.exe"; Parameters: "/u /s ""{app}\ArchiFlowShell.dll"""; Flags: runhidden

[InstallDelete]
; Очистка старых версий
Type: filesandordirs; Name: "{app}\data\cache"
Type: filesandordirs; Name: "{app}\temp"

[Code]
var
  DownloadPage: TDownloadWizardPage;

function IsVCRedistNeeded: Boolean;
var
  bNeedInstall: Boolean;
begin
  bNeedInstall := False;
  
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

function NextButtonClick(CurPageID: Integer): Boolean;
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
