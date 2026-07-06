// ThemeManager.h - UI Theme and styling
#pragma once

#include <windows.h>
#include <uxtheme.h>
#include <vssym32.h>
#include <map>
#include <string>

#pragma comment(lib, "uxtheme.lib")

class ThemeManager {
private:
    struct ThemeColors {
        COLORREF background;
        COLORREF foreground;
        COLORREF accent;
        COLORREF border;
        COLORREF highlight;
        COLORREF selectedBg;
        COLORREF selectedFg;
    };
    
    ThemeColors currentTheme;
    HTHEME hTheme;
    
public:
    enum Theme {
        THEME_LIGHT,
        THEME_DARK,
        THEME_SYSTEM
    };
    
    ThemeManager() : currentTheme({0}) {
        hTheme = OpenThemeData(NULL, VSCLASS_WINDOW);
    }
    
    ~ThemeManager() {
        if (hTheme) {
            CloseThemeData(hTheme);
        }
    }
    
    void SetTheme(Theme theme) {
        switch (theme) {
            case THEME_LIGHT:
                currentTheme = {
                    RGB(255, 255, 255), // Background
                    RGB(33, 33, 33),    // Foreground
                    RGB(0, 120, 215),   // Accent
                    RGB(200, 200, 200), // Border
                    RGB(229, 243, 255), // Highlight
                    RGB(0, 120, 215),   // Selected Background
                    RGB(255, 255, 255)  // Selected Foreground
                };
                break;
                
            case THEME_DARK:
                currentTheme = {
                    RGB(30, 30, 30),    // Background
                    RGB(240, 240, 240), // Foreground
                    RGB(0, 120, 215),   // Accent
                    RGB(60, 60, 60),    // Border
                    RGB(50, 50, 50),    // Highlight
                    RGB(0, 90, 158),    // Selected Background
                    RGB(255, 255, 255)  // Selected Foreground
                };
                break;
                
            case THEME_SYSTEM:
            default:
                currentTheme = {
                    GetSysColor(COLOR_WINDOW),
                    GetSysColor(COLOR_WINDOWTEXT),
                    GetSysColor(COLOR_HIGHLIGHT),
                    GetSysColor(COLOR_WINDOWFRAME),
                    GetSysColor(COLOR_HIGHLIGHT),
                    GetSysColor(COLOR_HIGHLIGHT),
                    GetSysColor(COLOR_HIGHLIGHTTEXT)
                };
                break;
        }
    }
    
    COLORREF GetBackgroundColor() const { return currentTheme.background; }
    COLORREF GetForegroundColor() const { return currentTheme.foreground; }
    COLORREF GetAccentColor() const { return currentTheme.accent; }
    COLORREF GetBorderColor() const { return currentTheme.border; }
    COLORREF GetHighlightColor() const { return currentTheme.highlight; }
    COLORREF GetSelectedBgColor() const { return currentTheme.selectedBg; }
    COLORREF GetSelectedFgColor() const { return currentTheme.selectedFg; }
    
    void ApplyThemeToWindow(HWND hwnd) {
        // Apply theme colors to window
        SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, 
            (LONG_PTR)CreateSolidBrush(currentTheme.background));
        
        // Enable visual styles if available
        if (hTheme) {
            SetWindowTheme(hwnd, L"Explorer", NULL);
        }
        
        InvalidateRect(hwnd, NULL, TRUE);
        UpdateWindow(hwnd);
    }
    
    void DrawThemedButton(HDC hdc, RECT& rect, const std::wstring& text, 
                          bool isHovered, bool isPressed, bool isDefault) {
        // Create gradient background
        TRIVERTEX vertex[2];
        vertex[0].x = rect.left;
        vertex[0].y = rect.top;
        vertex[0].Red = GetRValue(currentTheme.accent) << 8;
        vertex[0].Green = GetGValue(currentTheme.accent) << 8;
        vertex[0].Blue = GetBValue(currentTheme.accent) << 8;
        vertex[0].Alpha = 0x0000;
        
        vertex[1].x = rect.right;
        vertex[1].y = rect.bottom;
        
        if (isPressed) {
            vertex[1].Red = GetRValue(currentTheme.accent) << 7;
            vertex[1].Green = GetGValue(currentTheme.accent) << 7;
            vertex[1].Blue = GetBValue(currentTheme.accent) << 7;
        } else if (isHovered) {
            vertex[1].Red = GetRValue(currentTheme.accent) << 9;
            vertex[1].Green = GetGValue(currentTheme.accent) << 9;
            vertex[1].Blue = GetBValue(currentTheme.accent) << 9;
        } else {
            vertex[1].Red = GetRValue(currentTheme.accent) << 8;
            vertex[1].Green = GetGValue(currentTheme.accent) << 8;
            vertex[1].Blue = GetBValue(currentTheme.accent) << 8;
        }
        vertex[1].Alpha = 0x0000;
        
        GRADIENT_RECT gRect;
        gRect.UpperLeft = 0;
        gRect.LowerRight = 1;
        
        GdiGradientFill(hdc, vertex, 2, &gRect, 1, GRADIENT_FILL_RECT_H);
        
        // Draw border
        HPEN hPen = CreatePen(PS_SOLID, 1, currentTheme.border);
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        
        Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
        
        SelectObject(hdc, hOldPen);
        SelectObject(hdc, hOldBrush);
        DeleteObject(hPen);
        
        // Draw text
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, currentTheme.selectedFg);
        
        RECT textRect = rect;
        DrawTextW(hdc, text.c_str(), -1, &textRect, 
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    
    void DrawModernDialog(HWND hDlg, HDC hdc) {
        RECT clientRect;
        GetClientRect(hDlg, &clientRect);
        
        // Fill background
        HBRUSH hBrush = CreateSolidBrush(currentTheme.background);
        FillRect(hdc, &clientRect, hBrush);
        DeleteObject(hBrush);
        
        // Draw title bar area with gradient
        RECT titleRect = {0, 0, clientRect.right, 60};
        
        TRIVERTEX vertex[2];
        vertex[0].x = titleRect.left;
        vertex[0].y = titleRect.top;
        vertex[0].Red = GetRValue(currentTheme.accent) << 8;
        vertex[0].Green = GetGValue(currentTheme.accent) << 8;
        vertex[0].Blue = GetBValue(currentTheme.accent) << 8;
        vertex[0].Alpha = 0x0000;
        
        vertex[1].x = titleRect.right;
        vertex[1].y = titleRect.bottom;
        vertex[1].Red = GetRValue(currentTheme.accent) << 6;
        vertex[1].Green = GetGValue(currentTheme.accent) << 6;
        vertex[1].Blue = GetBValue(currentTheme.accent) << 6;
        vertex[1].Alpha = 0x0000;
        
        GRADIENT_RECT gRect;
        gRect.UpperLeft = 0;
        gRect.LowerRight = 1;
        
        GdiGradientFill(hdc, vertex, 2, &gRect, 1, GRADIENT_FILL_RECT_V);
    }
};