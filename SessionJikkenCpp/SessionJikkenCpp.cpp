#include <windows.h>
#include "framework.h"
#include "Wtsapi32.h"
#include "LogOnDesktop.h"
#include "SessionJikkenCpp.h"

#pragma comment( lib, "wtsapi32.lib" )

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

LogOnDesktop lod;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SESSIONJIKKENCPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SESSIONJIKKENCPP));
    MSG msg;

    // メイン メッセージ ループ:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SESSIONJIKKENCPP));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SESSIONJIKKENCPP);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
        return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:        
        // このプロセスが属するセッションのイベントだけを取る 
        WTSRegisterSessionNotification(hWnd, NOTIFY_FOR_THIS_SESSION);
        // 全セッションのイベントを取る
        //WTSRegisterSessionNotification(hWnd, NOTIFY_FOR_ALL_SESSIONS);
        break;

    case WM_WTSSESSION_CHANGE:
    {
        // このプロセスが属するセッションのID
        auto pid = GetCurrentProcessId();
        DWORD sessionId = 0;
        auto hr = ProcessIdToSessionId(pid, &sessionId);

        // 発生したセッションイベントのセッションID
        auto eventSessionId = lParam;

        auto txt = std::format(L"WM_WTSSESSION_CHANGE process session = {0}, event session = {1}", sessionId, eventSessionId);
        lod.WriteLine(txt);

        switch (lParam)
        {
        case WTS_CONSOLE_CONNECT:lod.WriteLine(L"WTS_CONSOLE_CONNECT"); break;
        case WTS_CONSOLE_DISCONNECT:lod.WriteLine(L"WTS_CONSOLE_DISCONNECT"); break;
        case WTS_REMOTE_CONNECT:lod.WriteLine(L"WTS_REMOTE_CONNECT"); break;
        case WTS_REMOTE_DISCONNECT:lod.WriteLine(L"WTS_REMOTE_DISCONNECT"); break;
        case WTS_SESSION_LOGON:lod.WriteLine(L"WTS_SESSION_LOGON"); break;
        case WTS_SESSION_LOGOFF:lod.WriteLine(L"WTS_SESSION_LOGOFF"); break;
        case WTS_SESSION_LOCK:lod.WriteLine(L"WTS_SESSION_LOCK"); break;
        case WTS_SESSION_UNLOCK:lod.WriteLine(L"WTS_SESSION_UNLOCK"); break;
        case WTS_SESSION_REMOTE_CONTROL:lod.WriteLine(L"WTS_SESSION_REMOTE_CONTROL"); break;
        }
        break;
    }

    case WM_DESTROY:
        WTSUnRegisterSessionNotification(hWnd);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// https://learn.microsoft.com/ja-jp/windows/win32/api/wtsapi32/nf-wtsapi32-wtsregistersessionnotification
// https://learn.microsoft.com/ja-jp/windows/win32/termserv/wm-wtssession-change

// https://learn.microsoft.com/ja-jp/windows/win32/devnotes/getting-the-session-id-of-the-current-process
