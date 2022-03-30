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
    static TCHAR str[100];

    static int count;

    static TCHAR mul[50];
    static TCHAR mulOut[1000];
    // static RECT rect;

    static int x, y, n = -1;
    static int spaceCnt, cnt = 0;

    switch (iMessage)
    {
    case WM_CREATE:
        CreateCaret(hWnd, NULL, 5, 15);
        ShowCaret(hWnd);
        count = 0;
        break;

    case WM_CHAR:
        if (wParam == VK_SPACE && spaceCnt == 0)
        {
            x = _ttoi(str);
            cnt = count;
            ++spaceCnt;
            str[count++] = wParam;
            str[count] = '\0';
        }
        else if (wParam == VK_SPACE && spaceCnt == 1)
        {
            y = _ttoi(str + cnt);
            cnt = count;
            str[count++] = wParam;
            str[count] = '\0';
        }
        else if (wParam == VK_RETURN)
        {
            if (str[0] == '0')
            {
                DestroyWindow(hWnd);
                break;
            }

            n = _ttoi(str + cnt);

            spaceCnt = 2;

            count = 0;
            str[count] = '\0';

            mulOut[0] = '\0';

            // rect.left = x;
            // rect.top = y;
            // rect.right = x + 270;
            // rect.bottom = y + 500;
        }
        else
        {
            str[count++] = wParam;
            str[count] = '\0';
        }
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_PAINT:
        hDC = BeginPaint(hWnd, &ps);
        GetTextExtentPoint32(hDC, str, lstrlen(str), &size);
        TextOut(hDC, 0, 0, str, lstrlen(str));
        SetCaretPos(size.cx, 0);

        if (spaceCnt == 2)
        {
            for (int i = 0; i < 19; ++i)
            {
                wsprintf(mul, L"%d*%d = %d ", n, i + 1, n * (i + 1));
                _tcscat(mulOut, mul);
            }

            int line = lstrlen(mulOut) / 40;
            int lastLetter = lstrlen(mulOut) % 40;

            for (int i = 0; i < line; ++i)
                TextOut(hDC, x, y + 15 * i, &mulOut[40 * i], 40);
            TextOut(hDC, x, y + 15 * line, &mulOut[40 * line], lastLetter);

            // DrawText(hDC, mulOut, lstrlen(mulOut), &rect, DT_WORDBREAK | DT_EDITCONTROL);
            spaceCnt = 0;
        }

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