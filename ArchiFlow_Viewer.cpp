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
#include <unordered_map>
#include <cstring>
#include <cstdio>
#include <functional>
#include <sys/stat.h>
#include <algorithm>
#include <exception>
#include <stack>
#include <mutex>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "shell32.lib")

#include <archive.h>
#include <archive_entry.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "shell32.lib")

#ifdef LANG_RUSSIAN
#undef LANG_RUSSIAN
#endif
#ifdef LANG_ENGLISH
#undef LANG_ENGLISH
#endif

#define IDC_FILE_LIST       1001
#define IDC_EXTRACT_BTN     1002
#define IDC_ADD_BTN         1003
#define IDC_DELETE_BTN      1004
#define IDC_OPEN_BTN        1006
#define IDC_STATUS_BAR      1005

#define IDM_NEW_ARCHIVE     40001
#define IDM_OPEN_ARCHIVE    40002
#define IDM_ADD_FILES       40003
#define IDM_EXTRACT_ALL     40004
#define IDM_EXTRACT_SEL     40005
#define IDM_DELETE_SEL      40006
#define IDM_EXIT            40007
#define IDM_ABOUT           40008
#define IDM_LANGUAGE_RU     40009
#define IDM_LANGUAGE_EN     40010

enum AppLanguage { APP_LANG_RUSSIAN, APP_LANG_ENGLISH };

struct LocStrings {
    std::wstring title, file, newarc, openarc, add, extract, extractAll, extractSel, deleteSel, exit, help, about, lang, ru, en;
    std::wstring selArc, selFiles, selFolder, done, err, ready, opening, filesInArchive;
    std::wstring nameCol, sizeCol, packedCol, ratioCol, dateCol, typeCol;
};

std::unordered_map<AppLanguage, LocStrings> loc;
AppLanguage curLang = APP_LANG_RUSSIAN;

void initLoc() {
    loc[APP_LANG_RUSSIAN] = {
        L"ArchiFlow - Архиватор", L"Файл",
        L"Создать новый архив", L"Открыть архив",
        L"Добавить файлы в архив", L"Извлечь",
        L"Извлечь все", L"Извлечь выбранные", L"Удалить выбранные",
        L"Выход", L"Справка", L"О программе",
        L"Язык", L"Русский", L"Английский",
        L"Сохранить архив как", L"Выберите файлы",
        L"Выберите папку", L"Готово!", L"Ошибка!",
        L"Готов", L"Открытие архива...", L"файлов в архиве",
        L"Имя файла", L"Размер", L"Сжатый", L"%", L"Изменен", L"Тип"
    };
    loc[APP_LANG_ENGLISH] = {
        L"ArchiFlow - Archiver", L"File",
        L"Create New Archive", L"Open Archive",
        L"Add Files to Archive", L"Extract",
        L"Extract All", L"Extract Selected", L"Delete Selected",
        L"Exit", L"Help", L"About",
        L"Language", L"Russian", L"English",
        L"Save archive as", L"Select files",
        L"Select folder", L"Done!", L"Error!",
        L"Ready", L"Opening archive...", L"files in archive",
        L"File Name", L"Size", L"Packed", L"%", L"Modified", L"Type"
    };
}

const std::wstring& ls(const std::wstring& key) {
    static std::unordered_map<std::wstring, std::wstring> cache;
    static AppLanguage lastLang = static_cast<AppLanguage>(-1);
    
    if (curLang != lastLang) {
        cache.clear();
        lastLang = curLang;
    }
    
    auto it = cache.find(key);
    if (it != cache.end()) return it->second;
    
    auto& l = loc[curLang];
    std::wstring value;
    if (key == L"title") value = l.title;
    else if (key == L"file") value = l.file;
    else if (key == L"newarc") value = l.newarc;
    else if (key == L"openarc") value = l.openarc;
    else if (key == L"add") value = l.add;
    else if (key == L"extract") value = l.extract;
    else if (key == L"extractAll") value = l.extractAll;
    else if (key == L"extractSel") value = l.extractSel;
    else if (key == L"deleteSel") value = l.deleteSel;
    else if (key == L"exit") value = l.exit;
    else if (key == L"help") value = l.help;
    else if (key == L"about") value = l.about;
    else if (key == L"lang") value = l.lang;
    else if (key == L"ru") value = l.ru;
    else if (key == L"en") value = l.en;
    else if (key == L"selArc") value = l.selArc;
    else if (key == L"selFiles") value = l.selFiles;
    else if (key == L"selFolder") value = l.selFolder;
    else if (key == L"done") value = l.done;
    else if (key == L"err") value = l.err;
    else if (key == L"ready") value = l.ready;
    else if (key == L"opening") value = l.opening;
    else if (key == L"filesInArchive") value = l.filesInArchive;
    else if (key == L"nameCol") value = l.nameCol;
    else if (key == L"sizeCol") value = l.sizeCol;
    else if (key == L"packedCol") value = l.packedCol;
    else if (key == L"ratioCol") value = l.ratioCol;
    else if (key == L"dateCol") value = l.dateCol;
    else if (key == L"typeCol") value = l.typeCol;
    else value = L"";
    
    cache[key] = value;
    return cache[key];
}

struct ArchiveFileInfo {
    std::wstring name;
    uint64_t size = 0;
    uint64_t packedSize = 0;
    time_t mtime = 0;
    std::wstring type;
};

struct ApplicationState {
    HINSTANCE hInst = nullptr;
    HWND hMainWnd = nullptr, hListView = nullptr, hStatusBar = nullptr;
    HWND hAddBtn = nullptr, hExtractBtn = nullptr, hDeleteBtn = nullptr, hOpenBtn = nullptr;
    std::wstring currentArchivePath;
    bool archiveLoaded = false;
    std::vector<ArchiveFileInfo> archiveFiles;
};

ApplicationState app;

bool loadArchiveContents(const std::wstring& archivePath);
void refreshFileList();

void WriteCrashDump(const std::wstring& reason, const std::wstring& modulePath, struct _EXCEPTION_POINTERS* ExceptionInfo = NULL);

LONG WINAPI CrashHandler(struct _EXCEPTION_POINTERS* ExceptionInfo) {
    WCHAR modulePath[MAX_PATH];
    GetModuleFileNameW(NULL, modulePath, MAX_PATH);
    
    std::wstring reason;
    switch (ExceptionInfo->ExceptionRecord->ExceptionCode) {
        case EXCEPTION_ACCESS_VIOLATION: reason = L"EXCEPTION_ACCESS_VIOLATION"; break;
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: reason = L"EXCEPTION_ARRAY_BOUNDS_EXCEEDED"; break;
        case EXCEPTION_BREAKPOINT: reason = L"EXCEPTION_BREAKPOINT"; break;
        case EXCEPTION_DATATYPE_MISALIGNMENT: reason = L"EXCEPTION_DATATYPE_MISALIGNMENT"; break;
        case EXCEPTION_FLT_DIVIDE_BY_ZERO: reason = L"EXCEPTION_FLT_DIVIDE_BY_ZERO"; break;
        case EXCEPTION_ILLEGAL_INSTRUCTION: reason = L"EXCEPTION_ILLEGAL_INSTRUCTION"; break;
        case EXCEPTION_INT_DIVIDE_BY_ZERO: reason = L"EXCEPTION_INT_DIVIDE_BY_ZERO"; break;
        case EXCEPTION_INVALID_HANDLE: reason = L"EXCEPTION_INVALID_HANDLE"; break;
        case EXCEPTION_PRIV_INSTRUCTION: reason = L"EXCEPTION_PRIV_INSTRUCTION"; break;
        case EXCEPTION_SINGLE_STEP: reason = L"EXCEPTION_SINGLE_STEP"; break;
        case EXCEPTION_IN_PAGE_ERROR: reason = L"EXCEPTION_IN_PAGE_ERROR"; break;
        case EXCEPTION_GUARD_PAGE: reason = L"EXCEPTION_GUARD_PAGE"; break;
        case EXCEPTION_NONCONTINUABLE: reason = L"EXCEPTION_NONCONTINUABLE"; break;
        case EXCEPTION_INVALID_DISPOSITION: reason = L"EXCEPTION_INVALID_DISPOSITION"; break;
        case EXCEPTION_STACK_OVERFLOW: reason = L"EXCEPTION_STACK_OVERFLOW"; break;
        default: {
            WCHAR buf[64];
            swprintf(buf, 64, L"0x%08X", ExceptionInfo->ExceptionRecord->ExceptionCode);
            reason = buf;
            break;
        }
    }
    
    WriteCrashDump(reason, std::wstring(modulePath), ExceptionInfo);
    
    return EXCEPTION_EXECUTE_HANDLER;
}

void WriteCrashDump(const std::wstring& reason, const std::wstring& modulePath, struct _EXCEPTION_POINTERS* ExceptionInfo) {
    std::wstring dumpPath = modulePath;
    size_t pos = dumpPath.find_last_of(L'.');
    if (pos != std::wstring::npos) {
        dumpPath = dumpPath.substr(0, pos) + L"_crash.txt";
    } else {
        dumpPath = dumpPath + L"_crash.txt";
    }
    
    std::wofstream file;
    file.open(dumpPath.c_str());
    if (file) {
        file << L"ArchiFlow Crash Report\n";
        file << L"======================\n\n";
        file << L"Reason: " << reason << L"\n\n";
        
        SYSTEMTIME st;
        GetLocalTime(&st);
        file << L"Time: " << st.wDay << L"." << st.wMonth << L"." << st.wYear << L" " 
             << st.wHour << L":" << st.wMinute << L":" << st.wSecond << L"\n\n";
        
        if (ExceptionInfo) {
            CHAR buffer[1024];
            HMODULE hModule;
            if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                          (LPCSTR)ExceptionInfo->ExceptionRecord->ExceptionAddress, &hModule) && hModule) {
                GetModuleFileNameA(hModule, buffer, sizeof(buffer));
                file << L"Module: " << std::wstring(buffer, buffer + strlen(buffer)) << L"\n\n";
            }
        }
        
        file << L"ArchiFlow v1.2\n";
        file.close();
    }
}

std::wstring formatSize(uint64_t size) {
    WCHAR buf[64];
    if (size == 0) return L"0 B";
    if (size < 1024) swprintf(buf, 64, L"%llu B", size);
    else if (size < 1024 * 1024) swprintf(buf, 64, L"%.1f KB", size / 1024.0);
    else if (size < 1024LL * 1024 * 1024) swprintf(buf, 64, L"%.1f MB", size / (1024.0 * 1024.0));
    else swprintf(buf, 64, L"%.2f GB", size / (1024.0 * 1024.0 * 1024.0));
    return buf;
}

std::wstring formatDate(time_t t) {
    if (t == 0) return L"-";
    WCHAR buf[64];
    struct tm* tm = localtime(&t);
    wcsftime(buf, 64, L"%d.%m.%Y %H:%M", tm);
    return buf;
}

std::wstring getFileType(const std::wstring& name) {
    if (name.empty()) return L"Файл";
    size_t pos = name.find_last_of(L'.');
    if (pos == std::wstring::npos) return L"Файл";
    std::wstring ext = name.substr(pos + 1);
    if (ext.empty()) return L"Файл";
    for (auto& c : ext) c = towlower(c);
    if (ext == L"txt" || ext == L"rtf") return L"Текстовый документ";
    if (ext == L"pdf") return L"PDF документ";
    if (ext == L"jpg" || ext == L"jpeg") return L"Изображение JPEG";
    if (ext == L"png") return L"Изображение PNG";
    if (ext == L"zip" || ext == L"rar" || ext == L"7z") return L"Архив";
    if (ext == L"exe") return L"Приложение";
    if (ext == L"dll") return L"Библиотека";
    if (ext == L"cpp" || ext == L"h") return L"Исходный код C++";
    if (ext == L"cs") return L"Исходный код C#";
    if (ext == L"unity") return L"Сцена Unity";
    if (ext == L"res") return L"Файл-Рессурс";
    if (ext == L"trash") return L"Файл-Мусор";
    if (ext == L"doc") return L"Формат документов DOC";
    if (ext == L"dotx") return L"Шаблон документа";
    if (ext == L"docm") return L"Документ со встроенными макросами";
    if (ext == L"docx") return L"Документ Mircosoft Word";
    if (ext == L"pdf") return L"PDF-файл";
    if (ext == L"md") return L"Файл-Markdown";
    if (ext == L"bat") return L"Консольное приложение";
    if (ext == L"py") return L"Python-Файл";
    if (ext == L"exe") return L"Исполняемый файл Windows";
    if (ext == L"sh") return L"Сценарий Unix-OS";
    if (ext == L"rc") return L"Resource Script";
    if (ext == L"iss") return L"Inno Setup";
    return L"Файл " + ext;
}

bool loadArchiveContents(const std::wstring& archivePath) {
    if (archivePath.empty()) return false;
    
    struct archive* a = archive_read_new();
    if (!a) return false;
    
    archive_read_support_format_all(a);
    archive_read_support_filter_all(a);
    
    std::string ap(archivePath.begin(), archivePath.end());
    
    int r = archive_read_open_filename(a, ap.c_str(), 10240);
    if (r != ARCHIVE_OK) {
        archive_read_free(a);
        return false;
    }
    
    app.archiveFiles.clear();
    struct archive_entry* entry;
    int fileCount = 0;
    
    while (true) {
        r = archive_read_next_header(a, &entry);
        if (r == ARCHIVE_EOF) break;
        if (r != ARCHIVE_OK) { 
            archive_read_close(a); 
            archive_read_free(a); 
            app.archiveFiles.clear();
            return false; 
        }
        
        ArchiveFileInfo info;
        const char* pn = archive_entry_pathname(entry);
        if (pn && strlen(pn) > 0) {
            info.name = std::wstring(pn, pn + strlen(pn));
        } else {
            info.name = L"<unknown>";
        }
        
        la_int64_t fileSize = archive_entry_size(entry);
        info.packedSize = (fileSize > 0) ? static_cast<uint64_t>(fileSize) : 0;
        info.size = (fileSize > 0) ? static_cast<uint64_t>(fileSize) : 0;
        info.mtime = archive_entry_mtime(entry);
        info.type = getFileType(info.name);
        app.archiveFiles.push_back(info);
        fileCount++;
    }
    
    archive_read_close(a);
    archive_read_free(a);
    
    return true;
}

void refreshFileList() {
    if (!app.hListView) return;
    
    ListView_DeleteAllItems(app.hListView);
    
    if (app.archiveFiles.empty()) return;
    
    for (size_t i = 0; i < app.archiveFiles.size(); i++) {
        LVITEMW item = {0};
        item.mask = LVIF_TEXT;
        item.iItem = static_cast<int>(i);
        item.iSubItem = 0;
        item.pszText = const_cast<LPWSTR>(app.archiveFiles[i].name.c_str());
        if (ListView_InsertItem(app.hListView, &item) == -1) continue;
        
        std::wstring sizeStr = formatSize(app.archiveFiles[i].size);
        ListView_SetItemText(app.hListView, static_cast<int>(i), 1, const_cast<LPWSTR>(sizeStr.c_str()));
        
        std::wstring packedStr = formatSize(app.archiveFiles[i].packedSize);
        ListView_SetItemText(app.hListView, static_cast<int>(i), 2, const_cast<LPWSTR>(packedStr.c_str()));
        
        WCHAR ratioBuf[32];
        if (app.archiveFiles[i].size > 0 && app.archiveFiles[i].packedSize > 0) {
            swprintf(ratioBuf, 32, L"%.0f%%", 100.0 - (app.archiveFiles[i].packedSize * 100.0 / app.archiveFiles[i].size));
        } else {
            wcscpy(ratioBuf, L"-");
        }
        ListView_SetItemText(app.hListView, static_cast<int>(i), 3, ratioBuf);
        
        std::wstring dateStr = formatDate(app.archiveFiles[i].mtime);
        ListView_SetItemText(app.hListView, static_cast<int>(i), 4, const_cast<LPWSTR>(dateStr.c_str()));
        
        ListView_SetItemText(app.hListView, static_cast<int>(i), 5, const_cast<LPWSTR>(app.archiveFiles[i].type.c_str()));
    }
    
    WCHAR statusText[256];
    swprintf(statusText, 256, L"%zu %s", app.archiveFiles.size(), ls(L"filesInArchive").c_str());
    if (app.hStatusBar) SendMessageW(app.hStatusBar, SB_SETTEXT, 0, (LPARAM)statusText);
}

bool extractZipArchive(const std::wstring& zipFile, const std::wstring& outDir,
                        const std::vector<int>& selectedIndices = {}) {
    struct archive* a = archive_read_new();
    archive_read_support_format_zip(a);
    
    std::string zf(zipFile.begin(), zipFile.end());
    if (archive_read_open_filename(a, zf.c_str(), 10240) != ARCHIVE_OK) {
        archive_read_free(a);
        return false;
    }
    
    struct archive* ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, ARCHIVE_EXTRACT_TIME);
    archive_write_disk_set_standard_lookup(ext);
    
    struct archive_entry* entry;
    std::string od(outDir.begin(), outDir.end());
    int fileIndex = 0;
    
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        bool extract = selectedIndices.empty();
        if (!extract) {
            if (std::find(selectedIndices.begin(), selectedIndices.end(), fileIndex) != selectedIndices.end()) {
                extract = true;
            }
        }
        
        if (extract) {
            const char* pathname = archive_entry_pathname(entry);
            if (!pathname) { fileIndex++; continue; }
            
            std::string en(pathname);
            std::string fp = od + "\\" + en;
            archive_entry_set_pathname(entry, fp.c_str());
            
            archive_write_header(ext, entry);
            
            const void* buff;
            size_t size;
            la_int64_t offset;
            
            while (archive_read_data_block(a, &buff, &size, &offset) == ARCHIVE_OK) {
                archive_write_data_block(ext, buff, size, offset);
            }
        }
        fileIndex++;
    }
    
    archive_write_close(ext);
    archive_write_free(ext);
    archive_read_close(a);
    archive_read_free(a);
    return true;
}

std::wstring saveZipDialog(HWND hwnd) {
    OPENFILENAMEW ofn = {0};
    WCHAR file[512] = L"archiflow.zip";
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = file;
    ofn.nMaxFile = 512;
    ofn.lpstrFilter = L"ZIP архив (*.zip)\0*.zip\0";
    ofn.lpstrTitle = ls(L"selArc").c_str();
    ofn.Flags = OFN_OVERWRITEPROMPT;
    if (GetSaveFileNameW(&ofn)) return file;
    return L"";
}

std::vector<std::wstring> openFilesDialog(HWND hwnd) {
    OPENFILENAMEW ofn = {0};
    WCHAR file[8192] = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = file;
    ofn.nMaxFile = 8192;
    ofn.lpstrFilter = L"Все файлы (*.*)\0*.*\0";
    ofn.lpstrTitle = ls(L"selFiles").c_str();
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;
    
    std::vector<std::wstring> result;
    if (GetOpenFileNameW(&ofn)) {
        WCHAR* p = file;
        std::wstring dir = p;
        p += dir.length() + 1;
        if (*p) {
            while (*p) {
                result.push_back(dir + L"\\" + p);
                p += wcslen(p) + 1;
            }
        } else {
            result.push_back(dir);
        }
    }
    return result;
}

std::wstring openArchiveDialog(HWND hwnd) {
    OPENFILENAMEW ofn = {0};
    WCHAR file[512] = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = file;
    ofn.nMaxFile = 512;
    ofn.lpstrFilter = L"Архивы (*.zip;*.7z;*.rar;*.tar;*.gz)\0*.zip;*.7z;*.rar;*.tar;*.gz\0Все файлы (*.*)\0*.*\0";
    ofn.lpstrTitle = ls(L"openarc").c_str();
    ofn.Flags = OFN_FILEMUSTEXIST;
    if (GetOpenFileNameW(&ofn)) return file;
    return L"";
}

std::wstring selectFolderDialog(HWND hwnd) {
    BROWSEINFOW bi = {0};
    bi.hwndOwner = hwnd;
    bi.lpszTitle = ls(L"selFolder").c_str();
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
    if (pidl) {
        WCHAR path[MAX_PATH];
        SHGetPathFromIDListW(pidl, path);
        CoTaskMemFree(pidl);
        return path;
    }
    return L"";
}

void createMainMenu(HWND hwnd) {
    HMENU hMenu = CreateMenu();
    HMENU hFile = CreatePopupMenu();
    HMENU hExtract = CreatePopupMenu();
    HMENU hLang = CreatePopupMenu();
    HMENU hHelp = CreatePopupMenu();
    
    AppendMenuW(hFile, MF_STRING, IDM_NEW_ARCHIVE, ls(L"newarc").c_str());
    AppendMenuW(hFile, MF_STRING, IDM_OPEN_ARCHIVE, ls(L"openarc").c_str());
    AppendMenuW(hFile, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hFile, MF_STRING, IDM_EXIT, ls(L"exit").c_str());
    
    AppendMenuW(hExtract, MF_STRING, IDM_EXTRACT_ALL, ls(L"extractAll").c_str());
    AppendMenuW(hExtract, MF_STRING, IDM_EXTRACT_SEL, ls(L"extractSel").c_str());
    AppendMenuW(hExtract, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hExtract, MF_STRING, IDM_DELETE_SEL, ls(L"deleteSel").c_str());
    
    AppendMenuW(hLang, MF_STRING, IDM_LANGUAGE_RU, ls(L"ru").c_str());
    AppendMenuW(hLang, MF_STRING, IDM_LANGUAGE_EN, ls(L"en").c_str());
    
    AppendMenuW(hHelp, MF_STRING, IDM_ABOUT, ls(L"about").c_str());
    
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFile, ls(L"file").c_str());
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hExtract, ls(L"extract").c_str());
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hLang, ls(L"lang").c_str());
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hHelp, ls(L"help").c_str());
    
    SetMenu(hwnd, hMenu);
}

void createControls(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    
    app.hListView = CreateWindowW(WC_LISTVIEWW, L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_EDITLABELS | LVS_SINGLESEL,
        5, 5, rc.right - rc.left - 10, rc.bottom - rc.top - 65,
        hwnd, (HMENU)IDC_FILE_LIST, app.hInst, NULL);
    
    ListView_SetExtendedListViewStyle(app.hListView, 
        LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
    
    LVCOLUMNW lvc = {0};
    lvc.mask = LVCF_TEXT | LVCF_WIDTH;
    
    lvc.cx = 300; lvc.pszText = const_cast<LPWSTR>(ls(L"nameCol").c_str());
    ListView_InsertColumn(app.hListView, 0, &lvc);
    lvc.cx = 100; lvc.pszText = const_cast<LPWSTR>(ls(L"sizeCol").c_str());
    ListView_InsertColumn(app.hListView, 1, &lvc);
    lvc.cx = 100; lvc.pszText = const_cast<LPWSTR>(ls(L"packedCol").c_str());
    ListView_InsertColumn(app.hListView, 2, &lvc);
    lvc.cx = 60; lvc.pszText = const_cast<LPWSTR>(ls(L"ratioCol").c_str());
    ListView_InsertColumn(app.hListView, 3, &lvc);
    lvc.cx = 140; lvc.pszText = const_cast<LPWSTR>(ls(L"dateCol").c_str());
    ListView_InsertColumn(app.hListView, 4, &lvc);
    lvc.cx = 195; lvc.pszText = const_cast<LPWSTR>(ls(L"typeCol").c_str());
    ListView_InsertColumn(app.hListView, 5, &lvc);
    
    int btnY = rc.bottom - rc.top - 55;
    app.hAddBtn = CreateWindowW(L"BUTTON", L"+ Добавить",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        5, btnY, 110, 25, hwnd, (HMENU)IDC_ADD_BTN, app.hInst, NULL);
    
    app.hExtractBtn = CreateWindowW(L"BUTTON", L"Извлечь",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        120, btnY, 110, 25, hwnd, (HMENU)IDC_EXTRACT_BTN, app.hInst, NULL);
    
    app.hDeleteBtn = CreateWindowW(L"BUTTON", L"Удалить",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        235, btnY, 110, 25, hwnd, (HMENU)IDC_DELETE_BTN, app.hInst, NULL);
    
    app.hOpenBtn = CreateWindowW(L"BUTTON", L"Открыть",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        350, btnY, 110, 25, hwnd, (HMENU)IDC_OPEN_BTN, app.hInst, NULL);
    
    app.hStatusBar = CreateWindowW(STATUSCLASSNAMEW, L"",
        WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
        0, 0, 0, 0, hwnd, (HMENU)IDC_STATUS_BAR, app.hInst, NULL);
    
    int statusWidths[] = {rc.right - rc.left - 200, -1};
    SendMessageW(app.hStatusBar, SB_SETPARTS, 2, (LPARAM)statusWidths);
    SendMessageW(app.hStatusBar, SB_SETTEXT, 0, (LPARAM)ls(L"ready").c_str());
}

void updateUI() {
    createMainMenu(app.hMainWnd);
    DrawMenuBar(app.hMainWnd);
    
    LVCOLUMNW lvc = {0};
    lvc.mask = LVCF_TEXT;
    lvc.pszText = const_cast<LPWSTR>(ls(L"nameCol").c_str());
    ListView_SetColumn(app.hListView, 0, &lvc);
    lvc.pszText = const_cast<LPWSTR>(ls(L"sizeCol").c_str());
    ListView_SetColumn(app.hListView, 1, &lvc);
    lvc.pszText = const_cast<LPWSTR>(ls(L"packedCol").c_str());
    ListView_SetColumn(app.hListView, 2, &lvc);
    lvc.pszText = const_cast<LPWSTR>(ls(L"ratioCol").c_str());
    ListView_SetColumn(app.hListView, 3, &lvc);
    lvc.pszText = const_cast<LPWSTR>(ls(L"dateCol").c_str());
    ListView_SetColumn(app.hListView, 4, &lvc);
    lvc.pszText = const_cast<LPWSTR>(ls(L"typeCol").c_str());
    ListView_SetColumn(app.hListView, 5, &lvc);
}

// Добавление файлов в существующий архив
bool addFilesToExistingArchive(const std::wstring& zipPath, const std::vector<std::wstring>& newFiles) {
    // Извлекаем архив во временную папку
    WCHAR tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    std::wstring tempDir = std::wstring(tempPath) + L"ArchiFlow_Add\\";
    if (!CreateDirectoryW(tempDir.c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
        return false;
    }
    
    // Используем существующую функцию extractZipArchive
    if (!extractZipArchive(zipPath, tempDir)) {
        RemoveDirectoryW(tempDir.c_str());
        return false;
    }
    
    // Копируем новые файлы во временную папку
    for (const auto& f : newFiles) {
        DWORD attrs = GetFileAttributesW(f.c_str());
        if (attrs == INVALID_FILE_ATTRIBUTES || (attrs & FILE_ATTRIBUTE_DIRECTORY)) continue;
        size_t pos = f.find_last_of(L"\\/");
        std::wstring name = (pos != std::wstring::npos) ? f.substr(pos + 1) : f;
        CopyFileW(f.c_str(), (tempDir + name).c_str(), FALSE);
    }
    
    // Собираем все файлы из tempDir
    std::vector<std::wstring> allFiles, relativePaths;
    
    std::function<void(const std::wstring&)> collect = [&](const std::wstring& dir) {
        WIN32_FIND_DATAW fd;
        HANDLE hFind = FindFirstFileW((dir + L"*").c_str(), &fd);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) continue;
                std::wstring full = dir + fd.cFileName;
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    collect(full + L"\\");
                } else {
                    allFiles.push_back(full);
                    std::wstring rel = full.substr(tempDir.length());
                    while (!rel.empty() && (rel[0] == L'\\' || rel[0] == L'/')) rel = rel.substr(1);
                    relativePaths.push_back(rel);
                }
            } while (FindNextFileW(hFind, &fd));
            FindClose(hFind);
        }
    };
    collect(tempDir);
    
    if (allFiles.empty()) {
        RemoveDirectoryW(tempDir.c_str());
        return false;
    }
    
    // Удаляем старый архив
    DeleteFileW(zipPath.c_str());
    
    // Создаем новый архив
    struct archive* out = archive_write_new();
    archive_write_set_format_zip(out);
    
    std::string outPath(zipPath.begin(), zipPath.end());
    if (archive_write_open_filename(out, outPath.c_str()) != ARCHIVE_OK) {
        archive_write_free(out);
        RemoveDirectoryW(tempDir.c_str());
        return false;
    }
    
    for (size_t i = 0; i < allFiles.size(); ++i) {
        struct archive* disk = archive_read_disk_new();
        archive_read_disk_set_standard_lookup(disk);
        struct archive_entry* newEntry = archive_entry_new();
        
        std::string fs(allFiles[i].begin(), allFiles[i].end());
        std::string rel(relativePaths[i].begin(), relativePaths[i].end());
        for (char& c : rel) if (c == '\\') c = '/';
        
        archive_entry_copy_pathname(newEntry, rel.c_str());
        archive_read_disk_entry_from_file(disk, newEntry, -1, 0);
        archive_write_header(out, newEntry);
        
        std::ifstream in(fs, std::ios::binary);
        if (in) {
            char buf[8192];
            while (in.read(buf, sizeof(buf))) {
                archive_write_data(out, buf, in.gcount());
            }
            if (in.gcount() > 0) {
                archive_write_data(out, buf, in.gcount());
            }
        }
        
        archive_entry_free(newEntry);
        archive_read_free(disk);
    }
    
    archive_write_close(out);
    archive_write_free(out);
    
    // Удаляем временную папку
    std::function<void(const std::wstring&)> removeDir = [&](const std::wstring& dir) {
        WIN32_FIND_DATAW fd;
        HANDLE hFind = FindFirstFileW((dir + L"*").c_str(), &fd);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) continue;
                std::wstring full = dir + fd.cFileName;
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    removeDir(full + L"\\");
                } else {
                    SetFileAttributesW(full.c_str(), FILE_ATTRIBUTE_NORMAL);
                    DeleteFileW(full.c_str());
                }
            } while (FindNextFileW(hFind, &fd));
            FindClose(hFind);
        }
        RemoveDirectoryW(dir.c_str());
    };
    removeDir(tempDir);
    
    return true;
}

bool createZipArchive(const std::wstring& zipFile, const std::vector<std::wstring>& files) {
    if (files.empty()) return false;
    
    DeleteFileW(zipFile.c_str());
    
    struct archive* a = archive_write_new();
    archive_write_set_format_zip(a);
    
    std::string zf(zipFile.begin(), zipFile.end());
    if (archive_write_open_filename(a, zf.c_str()) != ARCHIVE_OK) {
        archive_write_free(a);
        return false;
    }
    
    for (const auto& f : files) {
        DWORD attrs = GetFileAttributesW(f.c_str());
        if (attrs == INVALID_FILE_ATTRIBUTES || (attrs & FILE_ATTRIBUTE_DIRECTORY)) continue;
        
        struct archive* disk = archive_read_disk_new();
        archive_read_disk_set_standard_lookup(disk);
        
        struct archive_entry* entry = archive_entry_new();
        std::string fs(f.begin(), f.end());
        
        // Только имя файла
        size_t pos = f.find_last_of(L"\\/");
        std::wstring name = (pos != std::wstring::npos) ? f.substr(pos + 1) : f;
        std::string ns(name.begin(), name.end());
        
        archive_entry_copy_pathname(entry, ns.c_str());
        archive_read_disk_entry_from_file(disk, entry, -1, 0);
        archive_write_header(a, entry);
        
        std::ifstream in(fs, std::ios::binary);
        if (in) {
            char buf[8192];
            while (in.read(buf, sizeof(buf)) || in.gcount() > 0) {
                archive_write_data(a, buf, in.gcount());
            }
        }
        
        archive_entry_free(entry);
        archive_read_free(disk);
    }
    
    archive_write_close(a);
    archive_write_free(a);
    return true;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE:
            app.hMainWnd = hwnd;
            initLoc();
            createMainMenu(hwnd);
            createControls(hwnd);
            break;
        
        case WM_SIZE: {
            if (!app.hListView || !app.hAddBtn || !app.hExtractBtn || !app.hDeleteBtn || !app.hOpenBtn) break;
            
            RECT rc;
            GetClientRect(hwnd, &rc);
            int width = rc.right - rc.left;
            int height = rc.bottom - rc.top;
            
            SetWindowPos(app.hListView, NULL, 5, 5, width - 10, height - 65, SWP_NOZORDER);
            SetWindowPos(app.hAddBtn, NULL, 5, height - 55, 110, 25, SWP_NOZORDER);
            SetWindowPos(app.hExtractBtn, NULL, 120, height - 55, 110, 25, SWP_NOZORDER);
            SetWindowPos(app.hDeleteBtn, NULL, 235, height - 55, 110, 25, SWP_NOZORDER);
            SetWindowPos(app.hOpenBtn, NULL, 350, height - 55, 110, 25, SWP_NOZORDER);
            
            SendMessageW(app.hStatusBar, WM_SIZE, 0, 0);
            break;
        }
        
        case WM_COMMAND:
            switch (LOWORD(wp)) {
                case IDM_LANGUAGE_RU:
                    curLang = APP_LANG_RUSSIAN;
                    updateUI();
                    SetWindowTextW(hwnd, ls(L"title").c_str());
                    if (app.archiveLoaded) refreshFileList();
                    break;
                    
                case IDM_LANGUAGE_EN:
                    curLang = APP_LANG_ENGLISH;
                    updateUI();
                    SetWindowTextW(hwnd, ls(L"title").c_str());
                    if (app.archiveLoaded) refreshFileList();
                    break;
                
                case IDM_NEW_ARCHIVE: {
                    std::wstring zip = saveZipDialog(hwnd);
                    if (!zip.empty()) {
                        auto files = openFilesDialog(hwnd);
                        if (!files.empty() && createZipArchive(zip, files)) {
                            app.currentArchivePath = zip;
                            app.archiveLoaded = true;
                            loadArchiveContents(zip);
                            refreshFileList();
                            SetWindowTextW(hwnd, (L"ArchiFlow - " + app.currentArchivePath).c_str());
                        }
                    }
                    break;
                }
                
                case IDM_OPEN_ARCHIVE: {
                    std::wstring zip = openArchiveDialog(hwnd);
                    if (!zip.empty()) {
                        if (loadArchiveContents(zip)) {
                            app.currentArchivePath = zip;
                            app.archiveLoaded = true;
                            refreshFileList();
                            SetWindowTextW(hwnd, (L"ArchiFlow - " + app.currentArchivePath).c_str());
                        }
                    }
                    break;
                }
                
                case IDM_EXTRACT_ALL:
                case IDC_EXTRACT_BTN: {
                    if (app.archiveLoaded) {
                        std::wstring dir = selectFolderDialog(hwnd);
                        if (!dir.empty() && extractZipArchive(app.currentArchivePath, dir)) {
                            MessageBoxW(hwnd, ls(L"done").c_str(), L"ArchiFlow", MB_OK);
                        }
                    }
                    break;
                }
                
                case IDM_EXTRACT_SEL: {
                    if (app.archiveLoaded) {
                        std::wstring dir = selectFolderDialog(hwnd);
                        if (!dir.empty()) {
                            std::vector<int> selected;
                            int count = ListView_GetItemCount(app.hListView);
                            for (int i = 0; i < count; i++) {
                                if (ListView_GetItemState(app.hListView, i, LVIS_SELECTED) & LVIS_SELECTED)
                                    selected.push_back(i);
                            }
                            if (!selected.empty() && extractZipArchive(app.currentArchivePath, dir, selected))
                                MessageBoxW(hwnd, ls(L"done").c_str(), L"ArchiFlow", MB_OK);
                        }
                    }
                    break;
                }
                
                case IDC_ADD_BTN:
                case IDM_ADD_FILES: {
                    if (app.archiveLoaded) {
                        auto files = openFilesDialog(hwnd);
                        if (!files.empty()) {
                            if (addFilesToExistingArchive(app.currentArchivePath, files)) {
                                loadArchiveContents(app.currentArchivePath);
                                MessageBoxW(hwnd, ls(L"done").c_str(), L"ArchiFlow", MB_OK);
                            } else {
                                MessageBoxW(hwnd, L"Ошибка добавления файлов.", L"ArchiFlow", MB_OK | MB_ICONERROR);
                            }
                        }
                    } else {
                        std::wstring zip = saveZipDialog(hwnd);
                        if (!zip.empty()) {
                            auto files = openFilesDialog(hwnd);
                            if (!files.empty() && createZipArchive(zip, files)) {
                                app.currentArchivePath = zip;
                                app.archiveLoaded = true;
                                loadArchiveContents(zip);
                                refreshFileList();
                                SetWindowTextW(hwnd, (L"ArchiFlow - " + app.currentArchivePath).c_str());
                            }
                        }
                     }
                     break;
                 }
                 
                 case IDC_OPEN_BTN: {
                     std::wstring zip = openArchiveDialog(hwnd);
                     if (!zip.empty()) {
                         if (loadArchiveContents(zip)) {
                             app.currentArchivePath = zip;
                             app.archiveLoaded = true;
                             refreshFileList();
                             SetWindowTextW(hwnd, (L"ArchiFlow - " + app.currentArchivePath).c_str());
                         }
                     }
                     break;
                 }
                 
                 case IDM_ABOUT:
                    MessageBoxW(hwnd, L"ArchiFlow v.1.2\nСоздан K1sh-M1sh\nЛегковесный архиватор\n\nЛицензия: MIT",
                        L"О программе", MB_OK | MB_ICONINFORMATION);
                    break;
                
                case IDM_EXIT:
                    DestroyWindow(hwnd);
                    break;
            }
            break;
        
        case WM_NOTIFY: {
            NMHDR* nmhdr = (NMHDR*)lp;
            if (nmhdr->idFrom == IDC_FILE_LIST && nmhdr->code == NM_DBLCLK) {
                int sel = ListView_GetNextItem(app.hListView, -1, LVNI_SELECTED);
                if (sel >= 0 && app.archiveLoaded) {
                    std::wstring dir = selectFolderDialog(hwnd);
                    if (!dir.empty()) {
                        std::vector<int> selected = {sel};
                        if (extractZipArchive(app.currentArchivePath, dir, selected))
                            MessageBoxW(hwnd, ls(L"done").c_str(), L"ArchiFlow", MB_OK);
                    }
                }
            }
            break;
        }

                case WM_DROPFILES: {
            HDROP hDrop = (HDROP)wp;
            if (hDrop) {
                UINT fileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);
                std::vector<std::wstring> files;
                
                for (UINT i = 0; i < fileCount; i++) {
                    UINT len = DragQueryFileW(hDrop, i, NULL, 0) + 1;
                    WCHAR* buf = (WCHAR*)GlobalAlloc(GPTR, len * sizeof(WCHAR));
                    if (buf) {
                        DragQueryFileW(hDrop, i, buf, len);
                        files.push_back(buf);
                        GlobalFree(buf);
                    }
                }
                
                if (!files.empty()) {
                    // Создаем архив с перетащенными файлами
                    std::wstring zipPath = saveZipDialog(hwnd);
                    if (!zipPath.empty()) {
                        if (createZipArchive(zipPath, files)) {
                            app.currentArchivePath = zipPath;
                            app.archiveLoaded = true;
                            loadArchiveContents(zipPath);
                            refreshFileList();
                            SetWindowTextW(hwnd, (L"ArchiFlow - " + app.currentArchivePath).c_str());
                        }
                    }
                }
                
                DragFinish(hDrop);
            }
            break;
        }

                case WM_USER + 100: {
            if (!app.hListView) break;
            
            LPWSTR pathCopy = (LPWSTR)lp;
            if (pathCopy) {
                std::wstring path(pathCopy);
                GlobalFree(pathCopy);
                
                // Создаем архив с перетащенными файлами
                std::vector<std::wstring> files = {path};
                std::wstring zipPath = saveZipDialog(hwnd);
                if (!zipPath.empty()) {
                    if (createZipArchive(zipPath, files)) {
                        app.currentArchivePath = zipPath;
                        app.archiveLoaded = true;
                        loadArchiveContents(zipPath);
                        refreshFileList();
                        SetWindowTextW(hwnd, (L"ArchiFlow - " + app.currentArchivePath).c_str());
                    }
                }
            }
            break;
        }
        
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        
        default:
            return DefWindowProcW(hwnd, msg, wp, lp);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hi, HINSTANCE hp, LPSTR cmd, int show) {
    app.hInst = hi;
    
    INITCOMMONCONTROLSEX icex = { sizeof(icex), ICC_WIN95_CLASSES | ICC_BAR_CLASSES };
    InitCommonControlsEx(&icex);
    
    // Регистрируем класс
    WNDCLASSEXW wc = { sizeof(wc) };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hi;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"ArchiFlowViewerWnd";
    
    if (!RegisterClassExW(&wc)) {
        return 1;
    }
    
    HWND hwnd = CreateWindowW(L"ArchiFlowViewerWnd", L"ArchiFlow - Архиватор",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        1000, 650, NULL, NULL, hi, NULL);
    
    if (!hwnd) {
        return 1;
    }
    
    ShowWindow(hwnd, show);
    UpdateWindow(hwnd);
    
    // Включаем Drag&Drop
    DragAcceptFiles(hwnd, TRUE);
    
    // Получаем аргументы
    LPWSTR* argv;
    int argc;
    argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    
    // Открываем файл (один файл через аргумент)
    if (argv && argc > 1) {
        for (int i = 1; i < argc; i++) {
            std::wstring arg(argv[i]);
            
            while (!arg.empty() && arg[0] == L'"') arg = arg.substr(1);
            while (!arg.empty() && arg.back() == L'"') arg.pop_back();
            
            if (arg.empty()) continue;
            
            DWORD attrs = GetFileAttributesW(arg.c_str());
            if (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
                LPWSTR pathCopy = (LPWSTR)GlobalAlloc(GPTR, (arg.length() + 1) * sizeof(WCHAR));
                if (pathCopy) {
                    wcscpy(pathCopy, arg.c_str());
                    SetWindowTextW(hwnd, arg.c_str());
                    PostMessage(hwnd, WM_USER + 100, 0, (LPARAM)pathCopy);
                }
                break;
            }
        }
    }
    
    if (argv) LocalFree(argv);
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}