#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <tchar.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Window Programming Lab";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    HWND hWnd;
    MSG Message;
    WNDCLASSEX WndClass;
    g_hInst = hInstance;

    WndClass.cbSize = sizeof(WndClass);
    WndClass.style = CS_HREDRAW | CS_VREDRAW;
    WndClass.lpfnWndProc = (WNDPROC)WndProc;
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    WndClass.hInstance = hInstance;
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    WndClass.lpszMenuName = NULL;
    WndClass.lpszClassName = lpszClass;
    WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassEx(&WndClass);

    hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600,
                        NULL, (HMENU)NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&Message, 0, 0, 0))
    {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }
    return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hDC;

    static SIZE size;
    static TCHAR str[10][31];

    static int line, count;
    static int loopLineBool;

    switch (iMessage)
    {
    case WM_CREATE:
        CreateCaret(hWnd, NULL, 5, 15);
        ShowCaret(hWnd);
        line = 0;
        count = 0;
        loopLineBool = 0;
        break;

    case WM_CHAR:
        hDC = GetDC(hWnd);

        if (line == 10)
        {
            count = 0;
            line = 0;
            loopLineBool = 1;
        }

        if (wParam == VK_RETURN)
        {
            count = 0;
            ++line;
        }
        else
        {
            str[line][count++] = wParam;
            if (loopLineBool == 0)
                str[line][count] = '\0';

            if (count == 30)
            {
                count = 0;
                ++line;
            }
        }

        InvalidateRect(hWnd, NULL, TRUE);

        ReleaseDC(hWnd, hDC);
        break;

    case WM_PAINT:
        hDC = BeginPaint(hWnd, &ps);

        for (int i = 0; i < 10; ++i)
            TextOut(hDC, 0, i * 20, str[i], lstrlen(str[i]));

        GetTextExtentPoint32(hDC, str[line], count, &size);     // str[line] 배열의 문자들을 count, 즉 현재 해당 줄에서 입력한 부분까지 계산.
        SetCaretPos(size.cx, line * 20);

        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        HideCaret(hWnd);
        DestroyCaret();
        PostQuitMessage(0);
        break;
    }

    return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}