// FileUtils.h - File and archive utilities
#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>

class FileUtils {
public:
    static bool FileExists(const std::wstring& path) {
        DWORD attribs = GetFileAttributesW(path.c_str());
        return (attribs != INVALID_FILE_ATTRIBUTES && 
                !(attribs & FILE_ATTRIBUTE_DIRECTORY));
    }
    
    static bool DirectoryExists(const std::wstring& path) {
        DWORD attribs = GetFileAttributesW(path.c_str());
        return (attribs != INVALID_FILE_ATTRIBUTES && 
                (attribs & FILE_ATTRIBUTE_DIRECTORY));
    }
    
    static bool CreateDirectory(const std::wstring& path) {
        return CreateDirectoryW(path.c_str(), NULL) != 0;
    }
    
    static bool DeleteFile(const std::wstring& path) {
        return DeleteFileW(path.c_str()) != 0;
    }
    
    static std::wstring GetFileName(const std::wstring& path) {
        size_t pos = path.find_last_of(L"\\/");
        if (pos != std::wstring::npos) {
            return path.substr(pos + 1);
        }
        return path;
    }
    
    static std::wstring GetFileExtension(const std::wstring& path) {
        size_t pos = path.find_last_of(L'.');
        if (pos != std::wstring::npos) {
            return path.substr(pos);
        }
        return L"";
    }
    
    static std::wstring GetFileSizeString(const std::wstring& path) {
        HANDLE hFile = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        
        if (hFile == INVALID_HANDLE_VALUE) {
            return L"Неизвестно";
        }
        
        LARGE_INTEGER fileSize;
        GetFileSizeEx(hFile, &fileSize);
        CloseHandle(hFile);
        
        return FormatSize(fileSize.QuadPart);
    }
    
    static std::wstring FormatSize(__int64 size) {
        if (size < 1024) {
            return std::to_wstring(size) + L" Б";
        } else if (size < 1024 * 1024) {
            return std::to_wstring(size / 1024) + L" КБ";
        } else if (size < 1024LL * 1024 * 1024) {
            WCHAR buffer[32];
            swprintf(buffer, 32, L"%.2f МБ", size / (1024.0 * 1024.0));
            return buffer;
        } else {
            WCHAR buffer[32];
            swprintf(buffer, 32, L"%.2f ГБ", size / (1024.0 * 1024.0 * 1024.0));
            return buffer;
        }
    }
    
    static bool IsArchiveFile(const std::wstring& path) {
        std::wstring ext = GetFileExtension(path);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);
        
        static const std::vector<std::wstring> archiveExtensions = {
            L".zip", L".7z", L".rar", L".tar", L".gz", 
            L".bz2", L".xz", L".lz", L".lzma", L".cab"
        };
        
        return std::find(archiveExtensions.begin(), archiveExtensions.end(), ext) 
               != archiveExtensions.end();
    }
    
    static std::vector<std::wstring> GetFilesInDirectory(const std::wstring& directory, 
                                                        bool recursive = false) {
        std::vector<std::wstring> files;
        
        std::wstring searchPath = directory + L"\\*";
        WIN32_FIND_DATAW findData;
        HANDLE hFind = FindFirstFileW(searchPath.c_str(), &findData);
        
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (wcscmp(findData.cFileName, L".") == 0 || 
                    wcscmp(findData.cFileName, L"..") == 0) {
                    continue;
                }
                
                std::wstring fullPath = directory + L"\\" + findData.cFileName;
                
                if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    if (recursive) {
                        auto subFiles = GetFilesInDirectory(fullPath, recursive);
                        files.insert(files.end(), subFiles.begin(), subFiles.end());
                    }
                } else {
                    files.push_back(fullPath);
                }
            } while (FindNextFileW(hFind, &findData));
            
            FindClose(hFind);
        }
        
        return files;
    }
};