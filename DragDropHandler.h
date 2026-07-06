// DragDropHandler.h - Drag and drop support
#pragma once

#include <windows.h>
#include <shellapi.h>
#include <vector>
#include <string>

class DragDropHandler {
private:
    HWND hTargetWindow;
    bool isDropTarget;
    
public:
    DragDropHandler(HWND hwnd) : hTargetWindow(hwnd), isDropTarget(false) {
        // Register as drop target
        DragAcceptFiles(hwnd, TRUE);
    }
    
    ~DragDropHandler() {
        if (isDropTarget) {
            DragAcceptFiles(hTargetWindow, FALSE);
        }
    }
    
    std::vector<std::wstring> ProcessDropMessage(HDROP hDrop) {
        std::vector<std::wstring> droppedFiles;
        
        UINT fileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);
        
        for (UINT i = 0; i < fileCount; i++) {
            WCHAR filePath[MAX_PATH];
            if (DragQueryFileW(hDrop, i, filePath, MAX_PATH) > 0) {
                droppedFiles.push_back(filePath);
            }
        }
        
        DragFinish(hDrop);
        return droppedFiles;
    }
    
    bool IsArchiveFile(const std::wstring& filePath) {
        std::wstring extension = filePath.substr(filePath.find_last_of(L"."));
        std::transform(extension.begin(), extension.end(), extension.begin(), ::towlower);
        
        static const std::vector<std::wstring> archiveExtensions = {
            L".zip", L".7z", L".rar", L".tar", L".gz", 
            L".bz2", L".xz", L".lz", L".lzma", L".cab"
        };
        
        return std::find(archiveExtensions.begin(), archiveExtensions.end(), extension) 
               != archiveExtensions.end();
    }
    
    void HandleDroppedFiles(const std::vector<std::wstring>& files) {
        // Separate archives from regular files
        std::vector<std::wstring> archives;
        std::vector<std::wstring> regularFiles;
        
        for (const auto& file : files) {
            if (IsArchiveFile(file)) {
                archives.push_back(file);
            } else {
                regularFiles.push_back(file);
            }
        }
        
        // Open archives
        for (const auto& archive : archives) {
            // Post message to open archive
            PostMessage(hTargetWindow, WM_COMMAND, IDM_OPEN_ARCHIVE, 0);
        }
        
        // Add regular files to archive
        if (!regularFiles.empty()) {
            // Show archive creation dialog
            std::vector<std::wstring>* pFiles = new std::vector<std::wstring>(regularFiles);
            PostMessage(hTargetWindow, WM_USER + 100, (WPARAM)pFiles, 0);
        }
    }
};