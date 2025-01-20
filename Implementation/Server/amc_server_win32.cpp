/*++

Copyright (C) 2022 Autodesk Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Autodesk Inc. nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


#include <string>
#include <iostream>
#include <memory>

#include "amc_server_win32.hpp"
#include "common_utils.hpp"
#include <shellscalingapi.h>

#ifndef __GNUC__
#include <SDKDDKVer.h>
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "amc_server_win32_resource.hpp"
#include <stdexcept>

#include <string>
#include <vector>



#include "common_importstream_native.hpp"

using namespace AMC;

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);


#define FONTSIZE_AT_96DPI 18

CServerWin32IO::CServerWin32IO()
    : m_pServer (nullptr)
{

}

CServerWin32IO::~CServerWin32IO()
{

}

void CServerWin32IO::logMessageString(const std::string& sMessage)
{
    if (m_pServer != nullptr) {
        m_pServer->logMessageString (sMessage);
    }
}

std::string CServerWin32IO::readConfigurationString(const std::string& sFileName)
{
    AMCCommon::CImportStream_Native importStream(sFileName);
    return importStream.readAsString();
}

void CServerWin32IO::setServer(CServer_Win32* pServer)
{
    m_pServer = pServer;
}

CServer_Win32::CServer_Win32(void* hInstance)
    : CServer(std::make_shared<CServerWin32IO> ()), m_hInstance(hInstance),
        m_hAccelTable (nullptr),
        m_hWnd (nullptr)
{
    auto pIO = std::dynamic_pointer_cast<CServerWin32IO> (getServerIO ());
    if (pIO == nullptr)
        throw std::runtime_error("invalid server IO");

    pIO->setServer(this);

    m_sWindowClassName = L"AMC_WIN32";

    m_hAccelTable = LoadAcceleratorsW((HINSTANCE)hInstance, MAKEINTRESOURCEW(IDC_AMC_WIN32));

}


CServer_Win32::~CServer_Win32()
{

}

void CServer_Win32::executeNonBlocking(const std::string& sConfigurationFileName)
{
    m_ServerThread = std::move (std::thread([this, sConfigurationFileName]() {
        this->executeBlocking(sConfigurationFileName);
    }));

}

void CServer_Win32::stopServerThread()
{
    stopListening();
    m_ServerThread.join();
}


ATOM CServer_Win32::registerWindowClass()
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEXW);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = (HINSTANCE)m_hInstance;
    wcex.hIcon = LoadIcon((HINSTANCE)m_hInstance, MAKEINTRESOURCE(IDI_AMC_WIN32));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_AMC_WIN32);
    wcex.lpszClassName = m_sWindowClassName.c_str();
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_AMC_WIN32));

    return RegisterClassExW(&wcex);
}

void CServer_Win32::initInstance(int nCmdShow)
{
    if (m_hWnd != nullptr)
        throw std::runtime_error("window instance already initialized");
    m_hWnd = CreateWindowW(L"AMC_WIN32", L"Autodesk Machine Control Framework", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, (HINSTANCE)m_hInstance, nullptr);


    if (m_hWnd == nullptr)
        throw std::runtime_error("invalid window instance");

    // Store the server pointer in the window's user data
    SetWindowLongPtrW((HWND)m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    ShowWindow((HWND) m_hWnd, nCmdShow);
    UpdateWindow((HWND)m_hWnd);

    return;
}


void CServer_Win32::logMessageString(const std::string& sMessage)
{
    if (m_hWnd != nullptr)
    {
        HWND hWndList = GetDlgItem((HWND)m_hWnd, ID_EDITCHILD);
        if (hWndList != nullptr)
        {
            std::wstring sString = AMCCommon::CUtils::UTF8toUTF16 (sMessage);
            int nID = (int) SendMessageW(hWndList, LB_ADDSTRING, 0, (LPARAM)sString.c_str());

            if (nID > 0) {
                SendMessageW(hWndList, LB_SETTOPINDEX, (WPARAM)nID - 1, 0);
            }
        }

    }


}

void* CServer_Win32::getAccelTable()
{
    return m_hAccelTable;
}

void launchBrowser(CServer_Win32 * pServer)
{
    try {
        if (pServer != nullptr) {
            // URL to open in the browser
            std::wstring wsURL = AMCCommon::CUtils::UTF8toUTF16(pServer->getServerURL());

            // Use ShellExecuteW to open the browser
            HINSTANCE result = ShellExecuteW(nullptr, L"open", wsURL.c_str(), nullptr, nullptr, SW_SHOWNORMAL);

            // Check the result
            if ((uintptr_t)result <= 32) // Values less than or equal to 32 indicate an error
            {
                throw std::runtime_error("Failed to launch the browser.");
            }
        }
        else {
            throw std::runtime_error("internal error: could not access server object!");
        }
    }
    
    catch (std::exception& E) {
        std::string sError(E.what());
        std::wstring wsError;
        if (AMCCommon::CUtils::UTF8StringIsValid(sError))
            wsError = AMCCommon::CUtils::UTF8toUTF16(sError);

        if (wsError.empty())
            wsError = L"a unknown error occured!";

        MessageBoxW(nullptr, wsError.c_str (), L"Error", MB_OK | MB_ICONERROR);
    }


}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{   
    static HFONT fontHandle = nullptr;

    CServer_Win32* pServer = reinterpret_cast<CServer_Win32*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

    HINSTANCE hInst = (HINSTANCE)GetWindowLongPtrW(hWnd, GWLP_HINSTANCE);

   
    switch (message)
    {
    case WM_CREATE:
    {
        UINT dpi = GetDpiForWindow(hWnd);

        // Calculate the initial font size based on DPI (example uses 12-point font)
        int fontSize = MulDiv(FONTSIZE_AT_96DPI, dpi, 96);

        fontHandle = CreateFontW(fontSize,
            0,
            0,
            0,
            FW_DONTCARE,           // Font weight
            FALSE,                 // Italic attribute option
            FALSE,                 // Underline attribute option
            FALSE,                 // Strikeout attribute option
            DEFAULT_CHARSET,       // Character set identifier
            OUT_TT_PRECIS,         // Output precision
            CLIP_DEFAULT_PRECIS,   // Clipping precision
            CLEARTYPE_QUALITY,     // Output quality
            DEFAULT_PITCH | FF_SWISS, // Pitch and family
            L"Courier New");


        HWND hwndListBox = CreateWindowExW(
            0, L"LISTBOX",   // predefined class 
            NULL,         // no window title 
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_DISABLENOSCROLL | LBS_NOTIFY | LBS_EXTENDEDSEL,
            0, 0, 0, 0,   // set size in WM_SIZE message 
            hWnd,         // parent window 
            (HMENU)ID_EDITCHILD,   // edit control ID 
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);        // pointer not needed 

        // Add text to the window. 
        SendMessage(hwndListBox, WM_SETFONT, (WPARAM)fontHandle, 0);


        return 0;

    }
    case WM_SHOWWINDOW:
    {
        HWND hwndList = GetDlgItem(hWnd, ID_EDITCHILD);
        SetFocus(hwndList);

        return TRUE;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBoxW(hInst, MAKEINTRESOURCEW(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case IDM_LAUNCHBROWSER:
            launchBrowser(pServer);
            break;
        case ID_EDITCHILD:
        {
            if (HIWORD(wParam) == LBN_SELCHANGE)
            {

                HWND hwndListBox = GetDlgItem(hWnd, ID_EDITCHILD);


                // Get the count of selected items
                int selCount = (int)SendMessage(hwndListBox, LB_GETSELCOUNT, 0, 0);

                if (selCount > 0)
                {
                    // Allocate memory for the selected item indices
                    std::vector<int> selIndices(selCount);
                    SendMessage(hwndListBox, LB_GETSELITEMS, (WPARAM)selCount, (LPARAM)selIndices.data());

                    // Concatenate the selected item texts
                    std::wstring combinedText;
                    for (int index : selIndices)
                    {
                        int length = (int)SendMessage(hwndListBox, LB_GETTEXTLEN, (WPARAM)index, 0);
                        if (length != LB_ERR)
                        {
                            std::wstring itemText(length, L'\0');
                            SendMessage(hwndListBox, LB_GETTEXT, (WPARAM)index, (LPARAM)itemText.data());
                            combinedText += itemText + L"\r\n";  // Add each item text followed by a new line
                        }
                    }

                    // Open the clipboard
                    if (OpenClipboard(hWnd))
                    {
                        // Clear the clipboard
                        EmptyClipboard();

                        // Allocate a global memory object for the text
                        HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (combinedText.size() + 1) * sizeof(wchar_t));
                        if (hglbCopy)
                        {
                            // Lock the handle and copy the text to the buffer
                            wchar_t* buffer = (wchar_t*)GlobalLock(hglbCopy);
                            if (buffer != nullptr) {
                                memcpy(buffer, combinedText.c_str(), (combinedText.size() + 1) * sizeof(wchar_t));
                            }
                            GlobalUnlock(hglbCopy);

                            // Place the handle on the clipboard
                            SetClipboardData(CF_UNICODETEXT, hglbCopy);
                        }
                        // Close the clipboard
                        CloseClipboard();
                    }

                    
                }
            }
        }
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    case WM_SIZE:
    {
        // Make the edit control the size of the window's client area. 
        HWND hwndListBox = GetDlgItem(hWnd, ID_EDITCHILD);
        MoveWindow(hwndListBox,
            0, 0,                  // starting x- and y-coordinates 
            LOWORD(lParam),        // width of client area 
            HIWORD(lParam),        // height of client area 
            TRUE);                 // repaint window 
        return 0;
    }

    case WM_DPICHANGED:
    {
        // Get the recommended new size and position of the window
        RECT* const prcNewWindow = (RECT*)lParam;
        SetWindowPos(hWnd,
            NULL,
            prcNewWindow->left,
            prcNewWindow->top,
            prcNewWindow->right - prcNewWindow->left,
            prcNewWindow->bottom - prcNewWindow->top,
            SWP_NOZORDER | SWP_NOACTIVATE);

        // Update font size
        // Recreate the font with the new DPI
        UINT dpi = HIWORD(wParam); // New DPI
        int fontSize = MulDiv(FONTSIZE_AT_96DPI, dpi, 96);
        if (fontHandle)
        {
            DeleteObject(fontHandle); // Delete the old font
            fontHandle = nullptr;
        }

        fontHandle = CreateFontW(
            -fontSize,             // Height of font
            0,                     // Width of font
            0,                     // Angle of escapement
            0,                     // Orientation angle
            FW_DONTCARE,           // Font weight
            FALSE,                 // Italic attribute option
            FALSE,                 // Underline attribute option
            FALSE,                 // Strikeout attribute option
            DEFAULT_CHARSET,       // Character set identifier
            OUT_TT_PRECIS,         // Output precision
            CLIP_DEFAULT_PRECIS,   // Clipping precision
            CLEARTYPE_QUALITY,     // Output quality
            DEFAULT_PITCH | FF_SWISS, // Pitch and family
            L"Courier New");          // Font family

        HWND hwndListBox = GetDlgItem(hWnd, ID_EDITCHILD);
        SendMessage(hwndListBox, WM_SETFONT, (WPARAM)fontHandle, TRUE);

        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
    }
    break;


    case WM_DESTROY:
        if (fontHandle)
        {
            DeleteObject(fontHandle); // Clean up the font object
            fontHandle = nullptr;
        }

        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


bool hasCommandLineArgument(LPWSTR lpCmdLine, const std::wstring& argument)
{
    // Convert the command line to a wstring for easier processing
    std::wstring cmdLine(lpCmdLine);

    // Check if the argument exists (case-sensitive comparison)
    if (cmdLine.find(argument) != std::wstring::npos)
    {
        return true;
    }
    return false;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);

    bool bAutoOpenBrowser = hasCommandLineArgument(lpCmdLine, L"/openbrowser");

    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

    auto pServer = std::make_shared<CServer_Win32> (hInstance);
    pServer->registerWindowClass();
    pServer->initInstance(nCmdShow);

    std::string sConfigurationFileName = "amc_server.xml";
    pServer->executeNonBlocking(sConfigurationFileName);

    MSG msg;

    bool bServiceHasBeenStarted = false;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, (HACCEL)pServer->getAccelTable (), &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Wait until service has been started to launch browser UI...
        if (!bServiceHasBeenStarted) {
            bServiceHasBeenStarted = pServer->getServiceHasBeenStarted();
            if (bServiceHasBeenStarted) {
                if (bAutoOpenBrowser)
                    launchBrowser(pServer.get());
            }
        }
    }

    pServer->stopServerThread();

    return (int) msg.wParam;
}




