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
#include <map>
#include <cstring>
#include <cstdio>
#include <sys/stat.h>

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

std::map<AppLanguage, LocStrings> loc;
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
    auto& l = loc[curLang];
    if (key == L"title") return l.title;
    if (key == L"file") return l.file;
    if (key == L"newarc") return l.newarc;
    if (key == L"openarc") return l.openarc;
    if (key == L"add") return l.add;
    if (key == L"extract") return l.extract;
    if (key == L"extractAll") return l.extractAll;
    if (key == L"extractSel") return l.extractSel;
    if (key == L"deleteSel") return l.deleteSel;
    if (key == L"exit") return l.exit;
    if (key == L"help") return l.help;
    if (key == L"about") return l.about;
    if (key == L"lang") return l.lang;
    if (key == L"ru") return l.ru;
    if (key == L"en") return l.en;
    if (key == L"selArc") return l.selArc;
    if (key == L"selFiles") return l.selFiles;
    if (key == L"selFolder") return l.selFolder;
    if (key == L"done") return l.done;
    if (key == L"err") return l.err;
    if (key == L"ready") return l.ready;
    if (key == L"opening") return l.opening;
    if (key == L"filesInArchive") return l.filesInArchive;
    if (key == L"nameCol") return l.nameCol;
    if (key == L"sizeCol") return l.sizeCol;
    if (key == L"packedCol") return l.packedCol;
    if (key == L"ratioCol") return l.ratioCol;
    if (key == L"dateCol") return l.dateCol;
    if (key == L"typeCol") return l.typeCol;
    static std::wstring empty;
    return empty;
}

HINSTANCE hInst;
HWND hMainWnd, hListView, hStatusBar, hAddBtn, hExtractBtn, hDeleteBtn;
std::wstring currentArchivePath;
bool archiveLoaded = false;

struct ArchiveFileInfo {
    std::wstring name;
    uint64_t size;
    uint64_t packedSize;
    time_t mtime;
    std::wstring type;
};

std::vector<ArchiveFileInfo> archiveFiles;

std::wstring formatSize(uint64_t size) {
    WCHAR buf[64];
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
    size_t pos = name.find_last_of(L'.');
    if (pos == std::wstring::npos) return L"Файл";
    std::wstring ext = name.substr(pos + 1);
    for (auto& c : ext) c = towlower(c);
    if (ext == L"txt") return L"Текстовый документ";
    if (ext == L"pdf") return L"PDF документ";
    if (ext == L"jpg" || ext == L"jpeg") return L"Изображение JPEG";
    if (ext == L"png") return L"Изображение PNG";
    if (ext == L"zip" || ext == L"rar" || ext == L"7z") return L"Архив";
    if (ext == L"exe") return L"Приложение";
    if (ext == L"dll") return L"Библиотека";
    if (ext == L"cpp" || ext == L"h") return L"Исходный код C++";
    return L"Файл " + ext;
}

bool loadArchiveContents(const std::wstring& archivePath) {
    if (archivePath.empty()) return false;
    
    struct archive* a = archive_read_new();
    if (!a) return false;
    
    archive_read_support_format_zip(a);
    archive_read_support_format_7zip(a);
    archive_read_support_format_rar(a);
    archive_read_support_format_tar(a);
    
    std::string ap(archivePath.begin(), archivePath.end());
    
    int r = archive_read_open_filename(a, ap.c_str(), 10240);
    if (r != ARCHIVE_OK) {
        archive_read_free(a);
        return false;
    }
    
    archiveFiles.clear();
    struct archive_entry* entry;
    
    while (true) {
        r = archive_read_next_header(a, &entry);
        if (r == ARCHIVE_EOF) break;
        if (r != ARCHIVE_OK) { 
            archive_read_close(a); 
            archive_read_free(a); 
            archiveFiles.clear();
            return false; 
        }
        
        ArchiveFileInfo info;
        const char* pn = archive_entry_pathname(entry);
        if (pn && strlen(pn) > 0) {
            info.name = std::wstring(pn, pn + strlen(pn));
        } else {
            info.name = L"<unknown>";
        }
        info.size = archive_entry_size(entry);
        info.packedSize = 0;
        info.mtime = archive_entry_mtime(entry);
        info.type = getFileType(info.name);
        archiveFiles.push_back(info);
    }
    
    archive_read_close(a);
    archive_read_free(a);
    return true;
}

void refreshFileList() {
    ListView_DeleteAllItems(hListView);
    
    for (size_t i = 0; i < archiveFiles.size(); i++) {
        LVITEMW item = {0};
        item.mask = LVIF_TEXT;
        item.iItem = i;
        item.iSubItem = 0;
        item.pszText = (LPWSTR)archiveFiles[i].name.c_str();
        ListView_InsertItem(hListView, &item);
        
        std::wstring sizeStr = formatSize(archiveFiles[i].size);
        ListView_SetItemText(hListView, i, 1, (LPWSTR)sizeStr.c_str());
        
        std::wstring packedStr = formatSize(archiveFiles[i].packedSize);
        ListView_SetItemText(hListView, i, 2, (LPWSTR)packedStr.c_str());
        
        WCHAR ratioBuf[32];
        if (archiveFiles[i].size > 0 && archiveFiles[i].packedSize > 0) {
            swprintf(ratioBuf, 32, L"%.0f%%", 100.0 - (archiveFiles[i].packedSize * 100.0 / archiveFiles[i].size));
        } else {
            wcscpy(ratioBuf, L"-");
        }
        ListView_SetItemText(hListView, i, 3, ratioBuf);
        
        std::wstring dateStr = formatDate(archiveFiles[i].mtime);
        ListView_SetItemText(hListView, i, 4, (LPWSTR)dateStr.c_str());
        
        ListView_SetItemText(hListView, i, 5, (LPWSTR)archiveFiles[i].type.c_str());
    }
    
    WCHAR statusText[256];
    swprintf(statusText, 256, L"%zu %s", archiveFiles.size(), ls(L"filesInArchive").c_str());
    SendMessageW(hStatusBar, SB_SETTEXT, 0, (LPARAM)statusText);
}

bool createZipArchive(const std::wstring& zipFile, const std::vector<std::wstring>& files) {
    struct archive* a = archive_write_new();
    archive_write_set_format_zip(a);
    
    std::string zf(zipFile.begin(), zipFile.end());
    if (archive_write_open_filename(a, zf.c_str()) != ARCHIVE_OK) {
        archive_write_free(a);
        return false;
    }
    
    for (const auto& f : files) {
        struct archive* disk = archive_read_disk_new();
        archive_read_disk_set_standard_lookup(disk);
        
        struct archive_entry* entry = archive_entry_new();
        std::string fs(f.begin(), f.end());
        
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
            for (int idx : selectedIndices) {
                if (idx == fileIndex) { extract = true; break; }
            }
        }
        
        if (extract) {
            std::string en(archive_entry_pathname(entry));
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
    WCHAR file[512] = L"archive.zip";
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
    ofn.lpstrTitle = L"Открыть архив";
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
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    
    hListView = CreateWindowW(WC_LISTVIEWW, L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_EDITLABELS | LVS_SINGLESEL,
        5, 5, width - 10, height - 65,
        hwnd, (HMENU)IDC_FILE_LIST, hInst, NULL);
    
    ListView_SetExtendedListViewStyle(hListView, 
        LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
    
    LVCOLUMNW lvc = {0};
    lvc.mask = LVCF_TEXT | LVCF_WIDTH;
    
    lvc.cx = 300; lvc.pszText = (LPWSTR)ls(L"nameCol").c_str();
    ListView_InsertColumn(hListView, 0, &lvc);
    lvc.cx = 100; lvc.pszText = (LPWSTR)ls(L"sizeCol").c_str();
    ListView_InsertColumn(hListView, 1, &lvc);
    lvc.cx = 100; lvc.pszText = (LPWSTR)ls(L"packedCol").c_str();
    ListView_InsertColumn(hListView, 2, &lvc);
    lvc.cx = 60; lvc.pszText = (LPWSTR)ls(L"ratioCol").c_str();
    ListView_InsertColumn(hListView, 3, &lvc);
    lvc.cx = 140; lvc.pszText = (LPWSTR)ls(L"dateCol").c_str();
    ListView_InsertColumn(hListView, 4, &lvc);
    lvc.cx = 195; lvc.pszText = (LPWSTR)ls(L"typeCol").c_str();
    ListView_InsertColumn(hListView, 5, &lvc);
    
    int btnY = height - 55;
    hAddBtn = CreateWindowW(L"BUTTON", L"+ Добавить",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        5, btnY, 110, 25, hwnd, (HMENU)IDC_ADD_BTN, hInst, NULL);
    
    hExtractBtn = CreateWindowW(L"BUTTON", L"Извлечь",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        120, btnY, 110, 25, hwnd, (HMENU)IDC_EXTRACT_BTN, hInst, NULL);
    
    hDeleteBtn = CreateWindowW(L"BUTTON", L"Удалить",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        235, btnY, 110, 25, hwnd, (HMENU)IDC_DELETE_BTN, hInst, NULL);
    
    hStatusBar = CreateWindowW(STATUSCLASSNAMEW, L"",
        WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
        0, 0, 0, 0, hwnd, (HMENU)IDC_STATUS_BAR, hInst, NULL);
    
    int statusWidths[] = {width - 200, -1};
    SendMessageW(hStatusBar, SB_SETPARTS, 2, (LPARAM)statusWidths);
    SendMessageW(hStatusBar, SB_SETTEXT, 0, (LPARAM)ls(L"ready").c_str());
}

void updateUI() {
    createMainMenu(hMainWnd);
    DrawMenuBar(hMainWnd);
    
    LVCOLUMNW lvc = {0};
    lvc.mask = LVCF_TEXT;
    lvc.pszText = (LPWSTR)ls(L"nameCol").c_str();
    ListView_SetColumn(hListView, 0, &lvc);
    lvc.pszText = (LPWSTR)ls(L"sizeCol").c_str();
    ListView_SetColumn(hListView, 1, &lvc);
    lvc.pszText = (LPWSTR)ls(L"packedCol").c_str();
    ListView_SetColumn(hListView, 2, &lvc);
    lvc.pszText = (LPWSTR)ls(L"ratioCol").c_str();
    ListView_SetColumn(hListView, 3, &lvc);
    lvc.pszText = (LPWSTR)ls(L"dateCol").c_str();
    ListView_SetColumn(hListView, 4, &lvc);
    lvc.pszText = (LPWSTR)ls(L"typeCol").c_str();
    ListView_SetColumn(hListView, 5, &lvc);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE:
            hMainWnd = hwnd;
            initLoc();
            createMainMenu(hwnd);
            createControls(hwnd);
            break;
        
        case WM_SIZE: {
            RECT rc;
            GetClientRect(hwnd, &rc);
            int width = rc.right - rc.left;
            int height = rc.bottom - rc.top;
            
            SetWindowPos(hListView, NULL, 5, 5, width - 10, height - 65, SWP_NOZORDER);
            SetWindowPos(hAddBtn, NULL, 5, height - 55, 110, 25, SWP_NOZORDER);
            SetWindowPos(hExtractBtn, NULL, 120, height - 55, 110, 25, SWP_NOZORDER);
            SetWindowPos(hDeleteBtn, NULL, 235, height - 55, 110, 25, SWP_NOZORDER);
            
            SendMessageW(hStatusBar, WM_SIZE, 0, 0);
            break;
        }
        
        case WM_COMMAND:
            switch (LOWORD(wp)) {
                case IDM_LANGUAGE_RU:
                    curLang = APP_LANG_RUSSIAN;
                    updateUI();
                    SetWindowTextW(hwnd, ls(L"title").c_str());
                    if (archiveLoaded) refreshFileList();
                    break;
                    
                case IDM_LANGUAGE_EN:
                    curLang = APP_LANG_ENGLISH;
                    updateUI();
                    SetWindowTextW(hwnd, ls(L"title").c_str());
                    if (archiveLoaded) refreshFileList();
                    break;
                
                case IDM_NEW_ARCHIVE: {
                    std::wstring zip = saveZipDialog(hwnd);
                    if (!zip.empty()) {
                        auto files = openFilesDialog(hwnd);
                        if (!files.empty() && createZipArchive(zip, files)) {
                            currentArchivePath = zip;
                            archiveLoaded = true;
                            loadArchiveContents(zip);
                            refreshFileList();
                            SetWindowTextW(hwnd, (L"ArchiFlow - " + currentArchivePath).c_str());
                        }
                    }
                    break;
                }
                
                case IDM_OPEN_ARCHIVE: {
                    std::wstring zip = openArchiveDialog(hwnd);
                    if (!zip.empty()) {
                        if (loadArchiveContents(zip)) {
                            currentArchivePath = zip;
                            archiveLoaded = true;
                            refreshFileList();
                            SetWindowTextW(hwnd, (L"ArchiFlow - " + currentArchivePath).c_str());
                        }
                    }
                    break;
                }
                
                case IDM_EXTRACT_ALL:
                case IDC_EXTRACT_BTN: {
                    if (archiveLoaded) {
                        std::wstring dir = selectFolderDialog(hwnd);
                        if (!dir.empty() && extractZipArchive(currentArchivePath, dir)) {
                            MessageBoxW(hwnd, ls(L"done").c_str(), L"ArchiFlow", MB_OK);
                        }
                    }
                    break;
                }
                
                case IDM_EXTRACT_SEL: {
                    if (archiveLoaded) {
                        std::wstring dir = selectFolderDialog(hwnd);
                        if (!dir.empty()) {
                            std::vector<int> selected;
                            int count = ListView_GetItemCount(hListView);
                            for (int i = 0; i < count; i++) {
                                if (ListView_GetItemState(hListView, i, LVIS_SELECTED) & LVIS_SELECTED)
                                    selected.push_back(i);
                            }
                            if (!selected.empty() && extractZipArchive(currentArchivePath, dir, selected))
                                MessageBoxW(hwnd, ls(L"done").c_str(), L"ArchiFlow", MB_OK);
                        }
                    }
                    break;
                }
                
                case IDC_ADD_BTN: {
                    if (archiveLoaded) {
                        MessageBoxW(hwnd, L"Добавление в существующий архив в разработке.", L"ArchiFlow", MB_OK);
                    } else {
                        std::wstring zip = saveZipDialog(hwnd);
                        if (!zip.empty()) {
                            auto files = openFilesDialog(hwnd);
                            if (!files.empty() && createZipArchive(zip, files)) {
                                currentArchivePath = zip;
                                archiveLoaded = true;
                                loadArchiveContents(zip);
                                refreshFileList();
                                SetWindowTextW(hwnd, (L"ArchiFlow - " + currentArchivePath).c_str());
                            }
                        }
                    }
                    break;
                }
                
                case IDM_ABOUT:
                    MessageBoxW(hwnd, L"ArchiFlow v1.0\nЛегкий архиватор\n\nZIP, 7Z, RAR, TAR\n\nlibarchive\n2024",
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
                int sel = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
                if (sel >= 0 && archiveLoaded) {
                    std::wstring dir = selectFolderDialog(hwnd);
                    if (!dir.empty()) {
                        std::vector<int> selected = {sel};
                        if (extractZipArchive(currentArchivePath, dir, selected))
                            MessageBoxW(hwnd, ls(L"done").c_str(), L"ArchiFlow", MB_OK);
                    }
                }
            }
            break;
        }

                case WM_USER + 100: {
            // Отложенная загрузка файла
            WCHAR path[MAX_PATH];
            GetWindowTextW(hwnd, path, MAX_PATH);
            
            if (wcslen(path) > 0 && wcscmp(path, L"ArchiFlow - Архиватор") != 0 && wcscmp(path, L"ArchiFlow - Archiver") != 0) {
                DWORD attrs = GetFileAttributesW(path);
                if (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
                    if (loadArchiveContents(path)) {
                        currentArchivePath = path;
                        archiveLoaded = true;
                        refreshFileList();
                        SetWindowTextW(hwnd, (L"ArchiFlow - " + currentArchivePath).c_str());
                    } else {
                        SetWindowTextW(hwnd, ls(L"title").c_str());
                    }
                } else {
                    SetWindowTextW(hwnd, ls(L"title").c_str());
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
    hInst = hi;
    
    INITCOMMONCONTROLSEX icex = { sizeof(icex), ICC_WIN95_CLASSES | ICC_BAR_CLASSES };
    InitCommonControlsEx(&icex);
    
    // Получаем аргументы ДО создания окна
    LPWSTR* argv;
    int argc;
    argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    
    std::wstring debugMsg = L"Argc: " + std::to_wstring(argc) + L"\n";
    if (argv) {
        for (int i = 0; i < argc; i++) {
            debugMsg += L"argv[" + std::to_wstring(i) + L"]: " + argv[i] + L"\n";
        }
    }
    
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
    
    // Отложенная загрузка файла через сообщение
    if (argv && argc > 1) {
        std::wstring filePath;
        for (int i = 1; i < argc; i++) {
            std::wstring arg(argv[i]);
            if (arg != L"-a" && arg != L"-x") filePath = arg;
        }
        
        while (!filePath.empty() && filePath[0] == L'"') filePath = filePath.substr(1);
        while (!filePath.empty() && filePath.back() == L'"') filePath.pop_back();
        
        if (!filePath.empty()) {
            // Отправляем путь к файлу через SetWindowLongPtr + PostMessage
            // Проще: сохраняем в глобальную переменную и используем таймер
            SetWindowTextW(hwnd, filePath.c_str()); // Временно сохраняем путь в заголовке
            PostMessage(hwnd, WM_USER + 100, 0, 0); // Сигнал для загрузки
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