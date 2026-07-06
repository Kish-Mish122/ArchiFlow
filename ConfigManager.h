// ConfigManager.h - Configuration management
#pragma once

#include <windows.h>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <shlobj.h>

class ConfigManager {
private:
    std::map<std::wstring, std::map<std::wstring, std::wstring>> sections;
    std::wstring configPath;
    
    std::wstring Trim(const std::wstring& str) {
        size_t first = str.find_first_not_of(L" \t\r\n");
        if (first == std::wstring::npos) return L"";
        size_t last = str.find_last_not_of(L" \t\r\n");
        return str.substr(first, last - first + 1);
    }
    
public:
    ConfigManager() {
        // Get config path in AppData
        WCHAR appDataPath[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, appDataPath))) {
            configPath = std::wstring(appDataPath) + L"\\ArchiFlow\\config.ini";
        }
    }
    
    bool LoadConfig() {
        sections.clear();
        
        std::wifstream file(configPath);
        if (!file.is_open()) {
            return false;
        }
        
        std::wstring currentSection;
        std::wstring line;
        
        while (std::getline(file, line)) {
            line = Trim(line);
            
            if (line.empty() || line[0] == ';') {
                continue;
            }
            
            if (line[0] == '[' && line[line.length() - 1] == ']') {
                currentSection = line.substr(1, line.length() - 2);
                continue;
            }
            
            size_t equalsPos = line.find('=');
            if (equalsPos != std::wstring::npos) {
                std::wstring key = Trim(line.substr(0, equalsPos));
                std::wstring value = Trim(line.substr(equalsPos + 1));
                
                if (!currentSection.empty() && !key.empty()) {
                    sections[currentSection][key] = value;
                }
            }
        }
        
        file.close();
        return true;
    }
    
    bool SaveConfig() {
        // Ensure directory exists
        WCHAR appDataPath[MAX_PATH];
        SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, appDataPath);
        std::wstring dirPath = std::wstring(appDataPath) + L"\\ArchiFlow";
        CreateDirectoryW(dirPath.c_str(), NULL);
        
        std::wofstream file(configPath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& section : sections) {
            file << L"[" << section.first << L"]" << std::endl;
            for (const auto& keyValue : section.second) {
                file << keyValue.first << L"=" << keyValue.second << std::endl;
            }
            file << std::endl;
        }
        
        file.close();
        return true;
    }
    
    std::wstring GetValue(const std::wstring& section, const std::wstring& key, 
                          const std::wstring& defaultValue = L"") {
        if (sections.find(section) != sections.end()) {
            if (sections[section].find(key) != sections[section].end()) {
                return sections[section][key];
            }
        }
        return defaultValue;
    }
    
    void SetValue(const std::wstring& section, const std::wstring& key, 
                  const std::wstring& value) {
        sections[section][key] = value;
    }
    
    int GetIntValue(const std::wstring& section, const std::wstring& key, 
                    int defaultValue = 0) {
        std::wstring strValue = GetValue(section, key);
        if (strValue.empty()) return defaultValue;
        
        return _wtoi(strValue.c_str());
    }
    
    bool GetBoolValue(const std::wstring& section, const std::wstring& key, 
                      bool defaultValue = false) {
        std::wstring strValue = GetValue(section, key);
        if (strValue.empty()) return defaultValue;
        
        std::transform(strValue.begin(), strValue.end(), strValue.begin(), ::towlower);
        return (strValue == L"1" || strValue == L"true" || strValue == L"yes");
    }
    
    void SetIntValue(const std::wstring& section, const std::wstring& key, int value) {
        SetValue(section, key, std::to_wstring(value));
    }
    
    void SetBoolValue(const std::wstring& section, const std::wstring& key, bool value) {
        SetValue(section, key, value ? L"1" : L"0");
    }
    
    // Application specific settings
    std::wstring GetLanguage() {
        return GetValue(L"General", L"Language", L"Russian");
    }
    
    void SetLanguage(const std::wstring& language) {
        SetValue(L"General", L"Language", language);
    }
    
    std::wstring GetDefaultFormat() {
        return GetValue(L"General", L"DefaultFormat", L"ZIP");
    }
    
    int GetCompressionLevel() {
        return GetIntValue(L"General", L"CompressionLevel", 2);
    }
    
    bool IsContextMenuEnabled() {
        return GetBoolValue(L"ContextMenu", L"ShowAddToArchive", true);
    }
    
    bool IsFileAssociationEnabled(const std::wstring& extension) {
        std::wstring key = L"Associate" + extension;
        std::transform(key.begin(), key.end(), key.begin(), ::toupper);
        return GetBoolValue(L"Association", key, false);
    }
    
    std::wstring GetTempPath() {
        return GetValue(L"Advanced", L"TempPath", L"");
    }
    
    int GetBufferSize() {
        return GetIntValue(L"Advanced", L"BufferSize", 8192);
    }
};