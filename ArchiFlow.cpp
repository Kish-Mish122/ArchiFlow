// ArchiFlow.cpp - Main application file
// ArchiFlow - Lightweight Archiver with Context Menu Integration
// Supports: ZIP, 7Z, RAR, TAR, GZIP, BZIP2

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shlobj.h>
#include <shellapi.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <thread>
#include <mutex>

// libarchive headers
#include <archive.h>
#include <archive_entry.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "archive.lib")

// Resource IDs
#define IDR_MAIN_MENU           101
#define IDI_MAIN_ICON           102
#define IDI_SMALL_ICON          103
#define IDD_ARCHIVE_DIALOG      200
#define IDD_EXTRACT_DIALOG      201
#define IDD_SETTINGS_DIALOG     202
#define IDD_PROGRESS_DIALOG     203
#define IDD_ABOUT_DIALOG        204

// Control IDs
#define IDC_FILE_LIST           1001
#define IDC_ADD_BUTTON          1002
#define IDC_EXTRACT_BUTTON      1003
#define IDC_COMPRESSION_COMBO   1004
#define IDC_LEVEL_COMBO         1005
#define IDC_PASSWORD_EDIT       1006
#define IDC_PROGRESS_BAR        1007
#define IDC_STATUS_TEXT         1008
#define IDC_FORMAT_COMBO        1009
#define IDC_OUTPUT_PATH         1010
#define IDC_BROWSE_BUTTON       1011
#define IDC_SPLIT_SIZE          1012
#define IDC_ENCRYPT_CHECK       1013

// Menu IDs
#define IDM_NEW_ARCHIVE         40001
#define IDM_OPEN_ARCHIVE        40002
#define IDM_ADD_FILES           40003
#define IDM_EXTRACT_ALL         40004
#define IDM_SETTINGS            40005
#define IDM_EXIT                40006
#define IDM_ABOUT               40007
#define IDM_LANGUAGE_RU         40008
#define IDM_LANGUAGE_EN         40009

// String tables for localization
enum Language {
    LANG_RUSSIAN,
    LANG_ENGLISH
};

struct LocalizationStrings {
    std::wstring appTitle;
    std::wstring fileMenu;
    std::wstring newArchive;
    std::wstring openArchive;
    std::wstring addFiles;
    std::wstring extractAll;
    std::wstring settings;
    std::wstring exit;
    std::wstring helpMenu;
    std::wstring about;
    std::wstring languageMenu;
    std::wstring russian;
    std::wstring english;
    std::wstring archiveFormat;
    std::wstring compressionLevel;
    std::wstring browse;
    std::wstring addToArchive;
    std::wstring extractArchive;
    std::wstring password;
    std::wstring encrypt;
    std::wstring progress;
    std::wstring processing;
    std::wstring complete;
    std::wstring error;
    std::wstring selectFiles;
    std::wstring selectFolder;
    std::wstring outputPath;
    std::wstring splitSize;
    std::wstring contextAdd;
    std::wstring contextExtract;
};

std::map<Language, LocalizationStrings> localization;
Language currentLanguage = LANG_RUSSIAN;

void InitializeLocalization() {
    // Russian localization
    localization[LANG_RUSSIAN] = {
        L"ArchiFlow - Архиватор",
        L"Файл",
        L"Новый архив",
        L"Открыть архив",
        L"Добавить файлы",
        L"Извлечь все",
        L"Настройки",
        L"Выход",
        L"Справка",
        L"О программе",
        L"Язык",
        L"Русский",
        L"Английский",
        L"Формат архива",
        L"Уровень сжатия",
        L"Обзор",
        L"Добавить в архив",
        L"Извлечь из архива",
        L"Пароль",
        L"Шифровать",
        L"Прогресс",
        L"Обработка",
        L"Завершено",
        L"Ошибка",
        L"Выберите файлы",
        L"Выберите папку",
        L"Путь для извлечения",
        L"Размер тома",
        L"Добавить в архив",
        L"Извлечь в текущую папку"
    };

    // English localization
    localization[LANG_ENGLISH] = {
        L"ArchiFlow - Archiver",
        L"File",
        L"New Archive",
        L"Open Archive",
        L"Add Files",
        L"Extract All",
        L"Settings",
        L"Exit",
        L"Help",
        L"About",
        L"Language",
        L"Russian",
        L"English",
        L"Archive Format",
        L"Compression Level",
        L"Browse",
        L"Add to Archive",
        L"Extract from Archive",
        L"Password",
        L"Encrypt",
        L"Progress",
        L"Processing",
        L"Complete",
        L"Error",
        L"Select Files",
        L"Select Folder",
        L"Extract to",
        L"Split Size",
        L"Add to archive",
        L"Extract here"
    };
}

std::wstring GetLocalizedString(const std::wstring& key) {
    auto& loc = localization[currentLanguage];
    if (key == L"appTitle") return loc.appTitle;
    if (key == L"fileMenu") return loc.fileMenu;
    if (key == L"newArchive") return loc.newArchive;
    if (key == L"openArchive") return loc.openArchive;
    if (key == L"addFiles") return loc.addFiles;
    if (key == L"extractAll") return loc.extractAll;
    if (key == L"settings") return loc.settings;
    if (key == L"exit") return loc.exit;
    if (key == L"helpMenu") return loc.helpMenu;
    if (key == L"about") return loc.about;
    if (key == L"languageMenu") return loc.languageMenu;
    if (key == L"russian") return loc.russian;
    if (key == L"english") return loc.english;
    if (key == L"archiveFormat") return loc.archiveFormat;
    if (key == L"compressionLevel") return loc.compressionLevel;
    if (key == L"browse") return loc.browse;
    if (key == L"addToArchive") return loc.addToArchive;
    if (key == L"extractArchive") return loc.extractArchive;
    if (key == L"password") return loc.password;
    if (key == L"encrypt") return loc.encrypt;
    if (key == L"progress") return loc.progress;
    if (key == L"processing") return loc.processing;
    if (key == L"complete") return loc.complete;
    if (key == L"error") return loc.error;
    if (key == L"selectFiles") return loc.selectFiles;
    if (key == L"selectFolder") return loc.selectFolder;
    if (key == L"outputPath") return loc.outputPath;
    if (key == L"splitSize") return loc.splitSize;
    if (key == L"contextAdd") return loc.contextAdd;
    if (key == L"contextExtract") return loc.contextExtract;
    return L"";
}

class ArchiveManager {
private:
    std::mutex progressMutex;
    int currentProgress;
    HWND progressWindow;

    static int progressCallback(void* clientData, __int64 total, __int64 processed) {
        ArchiveManager* manager = static_cast<ArchiveManager*>(clientData);
        if (total > 0) {
            int progress = static_cast<int>((processed * 100) / total);
            manager->updateProgress(progress);
        }
        return 0;
    }

    void updateProgress(int progress) {
        std::lock_guard<std::mutex> lock(progressMutex);
        if (progress != currentProgress) {
            currentProgress = progress;
            if (progressWindow) {
                PostMessage(progressWindow, WM_USER + 1, progress, 0);
            }
        }
    }

public:
    ArchiveManager() : currentProgress(0), progressWindow(nullptr) {}

    void setProgressWindow(HWND hwnd) {
        progressWindow = hwnd;
    }

    bool createArchive(const std::vector<std::wstring>& files,
                      const std::wstring& outputPath,
                      const std::wstring& format,
                      int compressionLevel,
                      const std::wstring& password = L"") {
        struct archive* a;
        struct archive_entry* entry;
        
        a = archive_write_new();
        
        if (format == L"ZIP") {
            archive_write_set_format_zip(a);
        } else if (format == L"7Z") {
            archive_write_set_format_7zip(a);
        } else if (format == L"TAR") {
            archive_write_set_format_pax_restricted(a);
        } else {
            archive_write_set_format_zip(a);
        }

        archive_write_set_bytes_per_block(a, 10240);
        
        if (!password.empty()) {
            archive_write_set_passphrase(a, 
                std::string(password.begin(), password.end()).c_str());
            archive_write_set_options(a, "zip:encryption=traditional");
        }

        std::string outputFile(outputPath.begin(), outputPath.end());
        if (archive_write_open_filename(a, outputFile.c_str()) != ARCHIVE_OK) {
            archive_write_free(a);
            return false;
        }

        for (const auto& filePath : files) {
            std::string fileStr(filePath.begin(), filePath.end());
            
            struct archive* disk = archive_read_disk_new();
            archive_read_disk_set_standard_lookup(disk);
            
            entry = archive_entry_new();
            archive_entry_copy_pathname(entry, fileStr.c_str());
            archive_read_disk_entry_from_file(disk, entry, -1, 0);
            
            archive_write_header(a, entry);
            
            std::ifstream file(fileStr, std::ios::binary);
            if (file.is_open()) {
                char buffer[8192];
                while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
                    archive_write_data(a, buffer, file.gcount());
                }
                file.close();
            }
            
            archive_entry_free(entry);
            archive_read_free(disk);
        }

        archive_write_close(a);
        archive_write_free(a);
        
        return true;
    }

    bool extractArchive(const std::wstring& archivePath,
                       const std::wstring& outputDir,
                       const std::wstring& password = L"") {
        struct archive* a;
        struct archive* ext;
        struct archive_entry* entry;
        int flags;
        int r;

        flags = ARCHIVE_EXTRACT_TIME;
        flags |= ARCHIVE_EXTRACT_PERM;
        flags |= ARCHIVE_EXTRACT_ACL;
        flags |= ARCHIVE_EXTRACT_FFLAGS;

        a = archive_read_new();
        archive_read_support_format_all(a);
        archive_read_support_filter_all(a);

        std::string archiveFile(archivePath.begin(), archivePath.end());
        
        if ((r = archive_read_open_filename(a, archiveFile.c_str(), 10240))) {
            archive_read_free(a);
            return false;
        }

        if (!password.empty()) {
            archive_read_add_passphrase(a, 
                std::string(password.begin(), password.end()).c_str());
        }

        ext = archive_write_disk_new();
        archive_write_disk_set_options(ext, flags);
        archive_write_disk_set_standard_lookup(ext);

        for (;;) {
            r = archive_read_next_header(a, &entry);
            if (r == ARCHIVE_EOF) break;
            if (r != ARCHIVE_OK) {
                if (r == ARCHIVE_WARN) {
                    continue;
                } else {
                    archive_read_free(a);
                    archive_write_free(ext);
                    return false;
                }
            }

            std::string outputPath(outputDir.begin(), outputDir.end());
            std::string entryPath = archive_entry_pathname(entry);
            
            if (!outputPath.empty() && outputPath.back() != '\\') {
                outputPath += '\\';
            }
            
            std::string fullPath = outputPath + entryPath;
            archive_entry_set_pathname(entry, fullPath.c_str());

            r = archive_write_header(ext, entry);
            if (r != ARCHIVE_OK) {
                archive_write_free(ext);
                archive_read_free(a);
                return false;
            }

            const void* buff;
            size_t size;
            la_int64_t offset;

            for (;;) {
                r = archive_read_data_block(a, &buff, &size, &offset);
                if (r == ARCHIVE_EOF) break;
                if (r != ARCHIVE_OK) {
                    archive_write_free(ext);
                    archive_read_free(a);
                    return false;
                }
                r = archive_write_data_block(ext, buff, size, offset);
                if (r != ARCHIVE_OK) {
                    archive_write_free(ext);
                    archive_read_free(a);
                    return false;
                }
            }
        }

        archive_write_close(ext);
        archive_write_free(ext);
        archive_read_close(a);
        archive_read_free(a);

        return true;
    }

    bool listArchiveContents(const std::wstring& archivePath,
                           std::vector<std::wstring>& fileList) {
        struct archive* a;
        struct archive_entry* entry;

        a = archive_read_new();
        archive_read_support_format_all(a);
        archive_read_support_filter_all(a);

        std::string archiveFile(archivePath.begin(), archivePath.end());
        
        if (archive_read_open_filename(a, archiveFile.c_str(), 10240) != ARCHIVE_OK) {
            archive_read_free(a);
            return false;
        }

        while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
            std::string pathname = archive_entry_pathname(entry);
            fileList.push_back(std::wstring(pathname.begin(), pathname.end()));
        }

        archive_read_free(a);
        return true;
    }
};

// Global variables
HINSTANCE hInst;
HWND hMainWindow;
HWND hFileListView;
ArchiveManager archiveManager;
std::vector<std::wstring> currentFiles;

// Window procedure declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ArchiveDialogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ExtractDialogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ProgressDialogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK SettingsDialogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK AboutDialogProc(HWND, UINT, WPARAM, LPARAM);

void CreateMainMenu(HWND hwnd) {
    HMENU hMenu = CreateMenu();
    HMENU hFileMenu = CreatePopupMenu();
    HMENU hLanguageMenu = CreatePopupMenu();
    HMENU hHelpMenu = CreatePopupMenu();

    AppendMenuW(hFileMenu, MF_STRING, IDM_NEW_ARCHIVE, 
        GetLocalizedString(L"newArchive").c_str());
    AppendMenuW(hFileMenu, MF_STRING, IDM_OPEN_ARCHIVE, 
        GetLocalizedString(L"openArchive").c_str());
    AppendMenuW(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hFileMenu, MF_STRING, IDM_ADD_FILES, 
        GetLocalizedString(L"addFiles").c_str());
    AppendMenuW(hFileMenu, MF_STRING, IDM_EXTRACT_ALL, 
        GetLocalizedString(L"extractAll").c_str());
    AppendMenuW(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hFileMenu, MF_STRING, IDM_SETTINGS, 
        GetLocalizedString(L"settings").c_str());
    AppendMenuW(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hFileMenu, MF_STRING, IDM_EXIT, 
        GetLocalizedString(L"exit").c_str());

    AppendMenuW(hLanguageMenu, MF_STRING, IDM_LANGUAGE_RU, 
        GetLocalizedString(L"russian").c_str());
    AppendMenuW(hLanguageMenu, MF_STRING, IDM_LANGUAGE_EN, 
        GetLocalizedString(L"english").c_str());
    CheckMenuRadioItem(hLanguageMenu, IDM_LANGUAGE_RU, IDM_LANGUAGE_EN, 
        currentLanguage == LANG_RUSSIAN ? IDM_LANGUAGE_RU : IDM_LANGUAGE_EN, MF_BYCOMMAND);

    AppendMenuW(hHelpMenu, MF_STRING, IDM_ABOUT, 
        GetLocalizedString(L"about").c_str());

    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, 
        GetLocalizedString(L"fileMenu").c_str());
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hLanguageMenu, 
        GetLocalizedString(L"languageMenu").c_str());
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, 
        GetLocalizedString(L"helpMenu").c_str());

    SetMenu(hwnd, hMenu);
}

void UpdateMenuLanguage(HWND hwnd) {
    HMENU hMenu = GetMenu(hwnd);
    DestroyMenu(hMenu);
    CreateMainMenu(hwnd);
    DrawMenuBar(hwnd);
}

bool RegisterArchiFlowClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex = {0};
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAIN_ICON));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"ArchiFlowMainWindow";
    wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL_ICON));

    return RegisterClassExW(&wcex) != 0;
}

void InitializeListView(HWND hwnd) {
    hFileListView = CreateWindowW(WC_LISTVIEWW, L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_EDITLABELS,
        10, 10, 780, 400,
        hwnd, (HMENU)IDC_FILE_LIST, hInst, NULL);

    ListView_SetExtendedListViewStyle(hFileListView, 
        LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

    LVCOLUMNW lvc = {0};
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lvc.cx = 300;
    lvc.pszText = (LPWSTR)L"Имя файла";
    ListView_InsertColumn(hFileListView, 0, &lvc);

    lvc.cx = 150;
    lvc.pszText = (LPWSTR)L"Размер";
    ListView_InsertColumn(hFileListView, 1, &lvc);

    lvc.cx = 150;
    lvc.pszText = (LPWSTR)L"Тип";
    ListView_InsertColumn(hFileListView, 2, &lvc);

    lvc.cx = 178;
    lvc.pszText = (LPWSTR)L"Изменен";
    ListView_InsertColumn(hFileListView, 3, &lvc);
}

bool CreateMainWindow(HINSTANCE hInstance, int nCmdShow) {
    hInst = hInstance;

    InitializeLocalization();

    if (!RegisterArchiFlowClass(hInstance)) {
        return false;
    }

    HWND hWnd = CreateWindowW(L"ArchiFlowMainWindow",
        GetLocalizedString(L"appTitle").c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        900, 600,
        nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) {
        return false;
    }

    hMainWindow = hWnd;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return true;
}

void ProcessCommandLine(HWND hwnd, LPTSTR lpCmdLine) {
    int numArgs;
    LPWSTR* args = CommandLineToArgvW(lpCmdLine, &numArgs);

    if (numArgs > 1) {
        if (wcscmp(args[1], L"-a") == 0 && numArgs > 2) {
            // Add to archive mode
            std::vector<std::wstring> files;
            for (int i = 2; i < numArgs; i++) {
                files.push_back(args[i]);
            }
            
            if (!files.empty()) {
                DialogBoxParamW(hInst, MAKEINTRESOURCE(IDD_ARCHIVE_DIALOG), 
                    hwnd, ArchiveDialogProc, (LPARAM)&files);
            }
        } else if (wcscmp(args[1], L"-x") == 0 && numArgs > 2) {
            // Extract mode
            DialogBoxParamW(hInst, MAKEINTRESOURCE(IDD_EXTRACT_DIALOG), 
                hwnd, ExtractDialogProc, (LPARAM)args[2]);
        } else {
            // Open archive
            archiveManager.listArchiveContents(args[1], currentFiles);
            
            ListView_DeleteAllItems(hFileListView);
            for (size_t i = 0; i < currentFiles.size(); i++) {
                LVITEMW item = {0};
                item.mask = LVIF_TEXT;
                item.iItem = i;
                item.pszText = (LPWSTR)currentFiles[i].c_str();
                ListView_InsertItem(hFileListView, &item);
            }
        }
    }

    LocalFree(args);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE: {
            CreateMainMenu(hWnd);
            InitializeListView(hWnd);
            
            // Process command line arguments
            LPWSTR cmdLine = GetCommandLineW();
            ProcessCommandLine(hWnd, cmdLine);
            break;
        }

        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            switch (wmId) {
                case IDM_NEW_ARCHIVE:
                case IDM_ADD_FILES: {
                    std::vector<std::wstring> files;
                    DialogBoxParamW(hInst, MAKEINTRESOURCE(IDD_ARCHIVE_DIALOG), 
                        hWnd, ArchiveDialogProc, (LPARAM)&files);
                    break;
                }

                case IDM_OPEN_ARCHIVE: {
                    OPENFILENAMEW ofn = {0};
                    WCHAR szFile[260] = {0};
                    
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hWnd;
                    ofn.lpstrFile = szFile;
                    ofn.nMaxFile = sizeof(szFile);
                    ofn.lpstrFilter = L"Archive Files\0*.zip;*.7z;*.rar;*.tar;*.gz;*.bz2\0All Files\0*.*\0";
                    ofn.nFilterIndex = 1;
                    ofn.lpstrFileTitle = NULL;
                    ofn.nMaxFileTitle = 0;
                    ofn.lpstrInitialDir = NULL;
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                    if (GetOpenFileNameW(&ofn)) {
                        archiveManager.listArchiveContents(szFile, currentFiles);
                        
                        ListView_DeleteAllItems(hFileListView);
                        for (size_t i = 0; i < currentFiles.size(); i++) {
                            LVITEMW item = {0};
                            item.mask = LVIF_TEXT;
                            item.iItem = i;
                            item.pszText = (LPWSTR)currentFiles[i].c_str();
                            ListView_InsertItem(hFileListView, &item);
                        }
                    }
                    break;
                }

                case IDM_EXTRACT_ALL: {
                    DialogBoxParamW(hInst, MAKEINTRESOURCE(IDD_EXTRACT_DIALOG), 
                        hWnd, ExtractDialogProc, NULL);
                    break;
                }

                case IDM_SETTINGS: {
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_SETTINGS_DIALOG), 
                        hWnd, SettingsDialogProc);
                    break;
                }

                case IDM_LANGUAGE_RU: {
                    currentLanguage = LANG_RUSSIAN;
                    UpdateMenuLanguage(hWnd);
                    SetWindowTextW(hWnd, GetLocalizedString(L"appTitle").c_str());
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                }

                case IDM_LANGUAGE_EN: {
                    currentLanguage = LANG_ENGLISH;
                    UpdateMenuLanguage(hWnd);
                    SetWindowTextW(hWnd, GetLocalizedString(L"appTitle").c_str());
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                }

                case IDM_ABOUT: {
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUT_DIALOG), 
                        hWnd, AboutDialogProc);
                    break;
                }

                case IDM_EXIT: {
                    DestroyWindow(hWnd);
                    break;
                }
            }
            break;
        }

        case WM_SIZE: {
            RECT rcClient;
            GetClientRect(hWnd, &rcClient);
            
            if (hFileListView) {
                SetWindowPos(hFileListView, NULL, 10, 10, 
                    rcClient.right - 20, rcClient.bottom - 20, SWP_NOZORDER);
            }
            break;
        }

        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }

        default:
            return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK ArchiveDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static std::vector<std::wstring>* filesToArchive = nullptr;

    switch (message) {
        case WM_INITDIALOG: {
            filesToArchive = reinterpret_cast<std::vector<std::wstring>*>(lParam);
            
            // Set localized text
            SetWindowTextW(hDlg, GetLocalizedString(L"addToArchive").c_str());
            SetWindowTextW(GetDlgItem(hDlg, IDC_ADD_BUTTON), 
                GetLocalizedString(L"addToArchive").c_str());
            SetWindowTextW(GetDlgItem(hDlg, IDC_BROWSE_BUTTON), 
                GetLocalizedString(L"browse").c_str());
            
            // Initialize format combo
            HWND hFormat = GetDlgItem(hDlg, IDC_FORMAT_COMBO);
            SendMessageW(hFormat, CB_ADDSTRING, 0, (LPARAM)L"ZIP");
            SendMessageW(hFormat, CB_ADDSTRING, 0, (LPARAM)L"7Z");
            SendMessageW(hFormat, CB_ADDSTRING, 0, (LPARAM)L"TAR");
            SendMessageW(hFormat, CB_ADDSTRING, 0, (LPARAM)L"GZIP");
            SendMessageW(hFormat, CB_SETCURSEL, 0, 0);

            // Initialize compression level combo
            HWND hLevel = GetDlgItem(hDlg, IDC_LEVEL_COMBO);
            SendMessageW(hLevel, CB_ADDSTRING, 0, (LPARAM)L"Без сжатия");
            SendMessageW(hLevel, CB_ADDSTRING, 0, (LPARAM)L"Быстрое");
            SendMessageW(hLevel, CB_ADDSTRING, 0, (LPARAM)L"Нормальное");
            SendMessageW(hLevel, CB_ADDSTRING, 0, (LPARAM)L"Максимальное");
            SendMessageW(hLevel, CB_SETCURSEL, 2, 0);
            
            return TRUE;
        }

        case WM_COMMAND: {
            if (LOWORD(wParam) == IDC_BROWSE_BUTTON) {
                OPENFILENAMEW ofn = {0};
                WCHAR szFile[260] = {0};
                
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hDlg;
                ofn.lpstrFile = szFile;
                ofn.nMaxFile = sizeof(szFile);
                ofn.lpstrFilter = L"ZIP files\0*.zip\0All files\0*.*\0";
                ofn.nFilterIndex = 1;
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

                if (GetSaveFileNameW(&ofn)) {
                    SetWindowTextW(GetDlgItem(hDlg, IDC_OUTPUT_PATH), szFile);
                }
            } else if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDC_ADD_BUTTON) {
                WCHAR outputPath[260];
                WCHAR password[256];
                int formatIndex = SendMessageW(GetDlgItem(hDlg, IDC_FORMAT_COMBO), 
                    CB_GETCURSEL, 0, 0);
                int levelIndex = SendMessageW(GetDlgItem(hDlg, IDC_LEVEL_COMBO), 
                    CB_GETCURSEL, 0, 0);
                bool encrypt = IsDlgButtonChecked(hDlg, IDC_ENCRYPT_CHECK) == BST_CHECKED;

                GetWindowTextW(GetDlgItem(hDlg, IDC_OUTPUT_PATH), outputPath, 260);
                GetWindowTextW(GetDlgItem(hDlg, IDC_PASSWORD_EDIT), password, 256);

                if (wcslen(outputPath) == 0) {
                    MessageBoxW(hDlg, L"Укажите путь для сохранения архива", 
                        L"Ошибка", MB_OK | MB_ICONERROR);
                    return TRUE;
                }

                std::wstring format;
                switch (formatIndex) {
                    case 0: format = L"ZIP"; break;
                    case 1: format = L"7Z"; break;
                    case 2: format = L"TAR"; break;
                    case 3: format = L"GZIP"; break;
                    default: format = L"ZIP";
                }

                std::wstring pass = encrypt ? password : L"";

                // Show progress dialog
                HWND hProgress = CreateDialog(hInst, 
                    MAKEINTRESOURCE(IDD_PROGRESS_DIALOG), hDlg, ProgressDialogProc);
                ShowWindow(hProgress, SW_SHOW);

                archiveManager.setProgressWindow(hProgress);

                bool success = false;
                if (filesToArchive && !filesToArchive->empty()) {
                    success = archiveManager.createArchive(*filesToArchive, 
                        outputPath, format, levelIndex, pass);
                }

                if (success) {
                    MessageBoxW(hDlg, GetLocalizedString(L"complete").c_str(), 
                        L"ArchiFlow", MB_OK | MB_ICONINFORMATION);
                    EndDialog(hDlg, IDOK);
                } else {
                    MessageBoxW(hDlg, GetLocalizedString(L"error").c_str(), 
                        L"ArchiFlow", MB_OK | MB_ICONERROR);
                }

                DestroyWindow(hProgress);
            } else if (LOWORD(wParam) == IDCANCEL) {
                EndDialog(hDlg, IDCANCEL);
            }
            return TRUE;
        }
    }
    return FALSE;
}

INT_PTR CALLBACK ExtractDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static std::wstring* archivePath = nullptr;

    switch (message) {
        case WM_INITDIALOG: {
            if (lParam) {
                archivePath = reinterpret_cast<std::wstring*>(lParam);
            }
            
            SetWindowTextW(hDlg, GetLocalizedString(L"extractArchive").c_str());
            SetWindowTextW(GetDlgItem(hDlg, IDC_EXTRACT_BUTTON), 
                GetLocalizedString(L"extractArchive").c_str());
            SetWindowTextW(GetDlgItem(hDlg, IDC_BROWSE_BUTTON), 
                GetLocalizedString(L"browse").c_str());
            
            return TRUE;
        }

        case WM_COMMAND: {
            if (LOWORD(wParam) == IDC_BROWSE_BUTTON) {
                BROWSEINFOW bi = {0};
                bi.lpszTitle = GetLocalizedString(L"selectFolder").c_str();
                bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

                LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
                if (pidl) {
                    WCHAR szPath[MAX_PATH];
                    if (SHGetPathFromIDListW(pidl, szPath)) {
                        SetWindowTextW(GetDlgItem(hDlg, IDC_OUTPUT_PATH), szPath);
                    }
                    CoTaskMemFree(pidl);
                }
            } else if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDC_EXTRACT_BUTTON) {
                WCHAR outputPath[260];
                WCHAR password[256];

                GetWindowTextW(GetDlgItem(hDlg, IDC_OUTPUT_PATH), outputPath, 260);
                GetWindowTextW(GetDlgItem(hDlg, IDC_PASSWORD_EDIT), password, 256);

                if (wcslen(outputPath) == 0) {
                    MessageBoxW(hDlg, L"Укажите путь для извлечения", 
                        L"Ошибка", MB_OK | MB_ICONERROR);
                    return TRUE;
                }

                if (archivePath) {
                    archiveManager.extractArchive(*archivePath, outputPath, password);
                }

                EndDialog(hDlg, IDOK);
            } else if (LOWORD(wParam) == IDCANCEL) {
                EndDialog(hDlg, IDCANCEL);
            }
            return TRUE;
        }
    }
    return FALSE;
}

INT_PTR CALLBACK ProgressDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_INITDIALOG: {
            SetWindowTextW(hDlg, GetLocalizedString(L"processing").c_str());
            SendMessageW(GetDlgItem(hDlg, IDC_PROGRESS_BAR), PBM_SETRANGE, 0, MAKELPARAM(0, 100));
            return TRUE;
        }

        case WM_USER + 1: {
            int progress = (int)wParam;
            SendMessageW(GetDlgItem(hDlg, IDC_PROGRESS_BAR), PBM_SETPOS, progress, 0);
            
            WCHAR statusText[100];
            swprintf(statusText, 100, L"%d%%", progress);
            SetWindowTextW(GetDlgItem(hDlg, IDC_STATUS_TEXT), statusText);
            
            if (progress >= 100) {
                DestroyWindow(hDlg);
            }
            return TRUE;
        }
    }
    return FALSE;
}

INT_PTR CALLBACK SettingsDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_INITDIALOG: {
            SetWindowTextW(hDlg, GetLocalizedString(L"settings").c_str());
            return TRUE;
        }

        case WM_COMMAND: {
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
                EndDialog(hDlg, LOWORD(wParam));
                return TRUE;
            }
            break;
        }
    }
    return FALSE;
}

INT_PTR CALLBACK AboutDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_INITDIALOG: {
            SetWindowTextW(hDlg, GetLocalizedString(L"about").c_str());
            SetWindowTextW(GetDlgItem(hDlg, IDOK), L"OK");
            return TRUE;
        }

        case WM_COMMAND: {
            if (LOWORD(wParam) == IDOK) {
                EndDialog(hDlg, IDOK);
                return TRUE;
            }
            break;
        }
    }
    return FALSE;
}

// Entry point
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                      LPWSTR lpCmdLine, int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);

    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icex);

    if (!CreateMainWindow(hInstance, nCmdShow)) {
        return FALSE;
    }

    // Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}