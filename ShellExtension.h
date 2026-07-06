// ShellExtension.h - Context menu shell extension
#pragma once

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <string>
#include <vector>

#pragma comment(lib, "shlwapi.lib")

class ArchiFlowContextMenu : public IShellExtInit, public IContextMenu {
protected:
    LONG m_cRef;
    std::vector<std::wstring> m_files;
    
public:
    ArchiFlowContextMenu() : m_cRef(1) {}
    
    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) {
        if (riid == IID_IShellExtInit || riid == IID_IUnknown) {
            *ppv = static_cast<IShellExtInit*>(this);
        } else if (riid == IID_IContextMenu) {
            *ppv = static_cast<IContextMenu*>(this);
        } else {
            *ppv = NULL;
            return E_NOINTERFACE;
        }
        AddRef();
        return S_OK;
    }
    
    STDMETHODIMP_(ULONG) AddRef() {
        return InterlockedIncrement(&m_cRef);
    }
    
    STDMETHODIMP_(ULONG) Release() {
        if (InterlockedDecrement(&m_cRef) == 0) {
            delete this;
            return 0;
        }
        return m_cRef;
    }
    
    // IShellExtInit method
    STDMETHODIMP Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID) {
        if (!pDataObj) return E_INVALIDARG;
        
        FORMATETC fe = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
        STGMEDIUM stm;
        
        if (FAILED(pDataObj->GetData(&fe, &stm))) return E_INVALIDARG;
        
        HDROP hDrop = static_cast<HDROP>(GlobalLock(stm.hGlobal));
        if (!hDrop) {
            ReleaseStgMedium(&stm);
            return E_INVALIDARG;
        }
        
        UINT nFiles = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);
        m_files.clear();
        
        for (UINT i = 0; i < nFiles; i++) {
            WCHAR szFile[MAX_PATH];
            if (DragQueryFileW(hDrop, i, szFile, MAX_PATH)) {
                m_files.push_back(szFile);
            }
        }
        
        GlobalUnlock(stm.hGlobal);
        ReleaseStgMedium(&stm);
        
        return S_OK;
    }
    
    // IContextMenu methods
    STDMETHODIMP QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, 
                                   UINT idCmdLast, UINT uFlags) {
        if (uFlags & CMF_DEFAULTONLY) return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);
        
        HMENU hSubMenu = CreatePopupMenu();
        
        // Add main menu item
        InsertMenuW(hMenu, indexMenu, MF_BYPOSITION | MF_STRING | MF_POPUP, 
                    (UINT_PTR)hSubMenu, L"ArchiFlow");
        
        // Add submenu items
        std::wstring addText = L"Добавить в архив";
        if (!m_files.empty()) {
            if (m_files.size() == 1) {
                // Single file or folder
                WCHAR name[MAX_PATH];
                lstrcpyW(name, PathFindFileNameW(m_files[0].c_str()));
                addText = L"Добавить '" + std::wstring(name) + L"' в архив";
            } else {
                addText = L"Добавить выбранные элементы в архив";
            }
        }
        
        InsertMenuW(hSubMenu, 0, MF_BYPOSITION | MF_STRING, idCmdFirst, addText.c_str());
        
        std::wstring extractText = L"Извлечь в текущую папку";
        InsertMenuW(hSubMenu, 1, MF_BYPOSITION | MF_STRING, idCmdFirst + 1, extractText.c_str());
        InsertMenuW(hSubMenu, 2, MF_BYPOSITION | MF_STRING, idCmdFirst + 2, L"Извлечь в...");
        
        SetMenuDefaultItem(hSubMenu, 0, FALSE);
        
        return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 3);
    }
    
    STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO pici) {
        if (HIWORD(pici->lpVerb)) return E_INVALIDARG;
        
        UINT idCmd = LOWORD(pici->lpVerb);
        
        if (idCmd < 3 && !m_files.empty()) {
            // Build command line
            std::wstring cmdLine;
            WCHAR appPath[MAX_PATH];
            GetModuleFileNameW(NULL, appPath, MAX_PATH);
            
            if (idCmd == 0) {
                // Add to archive
                cmdLine = L"\"" + std::wstring(appPath) + L"\" -a";
                for (const auto& file : m_files) {
                    cmdLine += L" \"" + file + L"\"";
                }
            } else if (idCmd == 1) {
                // Extract here
                WCHAR currentDir[MAX_PATH];
                GetCurrentDirectoryW(MAX_PATH, currentDir);
                cmdLine = L"\"" + std::wstring(appPath) + L"\" -x \"" + 
                          m_files[0] + L"\" -o \"" + currentDir + L"\"";
            } else if (idCmd == 2) {
                // Extract to...
                cmdLine = L"\"" + std::wstring(appPath) + L"\" -x \"" + m_files[0] + L"\"";
            }
            
            // Execute command
            STARTUPINFOW si = { sizeof(STARTUPINFOW) };
            PROCESS_INFORMATION pi = { 0 };
            
            CreateProcessW(NULL, &cmdLine[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
            
            if (pi.hProcess) {
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
            
            return S_OK;
        }
        
        return E_INVALIDARG;
    }
    
    STDMETHODIMP GetCommandString(UINT_PTR idCmd, UINT uType, UINT* pReserved, 
                                   CHAR* pszName, UINT cchMax) {
        if (uType == GCS_HELPTEXTW) {
            switch (idCmd) {
                case 0:
                    lstrcpynW((LPWSTR)pszName, L"Добавить выбранные элементы в архив", cchMax);
                    break;
                case 1:
                    lstrcpynW((LPWSTR)pszName, L"Извлечь архив в текущую папку", cchMax);
                    break;
                case 2:
                    lstrcpynW((LPWSTR)pszName, L"Извлечь архив в указанную папку", cchMax);
                    break;
            }
            return S_OK;
        }
        return E_NOTIMPL;
    }
};

// Shell extension factory
class ArchiFlowContextMenuFactory : public IClassFactory {
protected:
    LONG m_cRef;
    
public:
    ArchiFlowContextMenuFactory() : m_cRef(1) {}
    
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) {
        if (riid == IID_IClassFactory || riid == IID_IUnknown) {
            *ppv = static_cast<IClassFactory*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    
    STDMETHODIMP_(ULONG) AddRef() {
        return InterlockedIncrement(&m_cRef);
    }
    
    STDMETHODIMP_(ULONG) Release() {
        if (InterlockedDecrement(&m_cRef) == 0) {
            delete this;
            return 0;
        }
        return m_cRef;
    }
    
    STDMETHODIMP CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObj) {
        if (pUnkOuter) return CLASS_E_NOAGGREGATION;
        
        ArchiFlowContextMenu* pMenu = new ArchiFlowContextMenu();
        if (!pMenu) return E_OUTOFMEMORY;
        
        HRESULT hr = pMenu->QueryInterface(riid, ppvObj);
        pMenu->Release();
        return hr;
    }
    
    STDMETHODIMP LockServer(BOOL fLock) {
        return E_NOTIMPL;
    }
};