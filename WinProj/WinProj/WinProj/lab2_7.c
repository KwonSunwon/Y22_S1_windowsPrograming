#define _CRT_SECURE_NO_WARNINGS

#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
// debuging console

#include <stdio.h>

#include <Windows.h>
#include <tchar.h>

#define MAX_LINE_NUMBER 10
#define MAX_COUNT_NUMBER 81

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
    static TCHAR str[MAX_LINE_NUMBER][MAX_COUNT_NUMBER];

    static int line, count;
    static int keyDownBool;
    static int insertToggle, f1Toggle;

    switch (iMessage)
    {
    case WM_CREATE:
        CreateCaret(hWnd, NULL, 5, 15);
        ShowCaret(hWnd);
        line = 0;
        count = 0;
        keyDownBool = 1;
        insertToggle = 0;
        f1Toggle = 0;
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            line = 0;
            count = 0;
            insertToggle = 0;
            f1Toggle = 0;
            keyDownBool = 1;

            memset((BYTE *)str, '\0', sizeof(TCHAR) * (MAX_LINE_NUMBER * MAX_COUNT_NUMBER));

            keyDownBool = 0;
            break;

        case VK_BACK:
            if (count == 0)
            {
                if (line == 0)
                {
                    keyDownBool = 0;
                    break;
                }
                --line;
                count = lstrlen(str[line]);
            }
            else
            {
                --count;
                for (int i = count; i < MAX_COUNT_NUMBER - 1; ++i)
                {
                    if (str[line][i] == '\0')
                        break;
                    str[line][i] = str[line][i + 1];
                }
            }
            InvalidateRect(hWnd, NULL, TRUE);
            keyDownBool = 0;
            break;

        case VK_DELETE:
            // printf("delete ÁøÀÔ\n");
            int idx = count;
            int firstIdx;
            int lastIdx;

            if (lstrlen(str[line]) == 0)
                break;
            else
            {
                while (str[line][idx] != ' ' && idx != -1)
                    --idx;
                firstIdx = ++idx;
                // printf("firstIdx %d\n", firstIdx);

                while (str[line][idx] != ' ' && str[line][idx] != '\0')
                    ++idx;
                lastIdx = idx;
                // printf("lastIdx %d\n", lastIdx);
            }

            for (int i = firstIdx; i < lastIdx + 1; ++i)
            {
                for (int j = firstIdx; j < MAX_COUNT_NUMBER - 1; ++j)
                {
                    str[line][j] = str[line][j + 1];
                    // printf("%d %d : %ls\n", i, j, str[line]);
                }
            }

            count = lstrlen(str[line]);

            InvalidateRect(hWnd, NULL, TRUE);
            break;

        case VK_HOME:
            count = 0;
            break;

        case VK_END:
            count = lstrlen(str[line]);
            break;

        case VK_LEFT:
            if (count == 0)
                break;
            --count;
            break;

        case VK_RIGHT:
            if (count == MAX_COUNT_NUMBER - 1)
                break;
            if (count >= lstrlen(str[line]))
            {
                count = lstrlen(str[line]);
                break;
            }
            ++count;
            break;

        case VK_UP:
            if (line == 0)
                break;
            if (count > lstrlen(str[line - 1]))
                count = lstrlen(str[line - 1]);
            --line;
            break;

        case VK_DOWN:
            if (line == MAX_LINE_NUMBER - 1)
                break;
            if (count > lstrlen(str[line + 1]))
                count = lstrlen(str[line + 1]);
            ++line;
            InvalidateRect(hWnd, NULL, TRUE);
            break;

        case VK_INSERT:
            if (insertToggle)
                insertToggle = 0;
            else
                insertToggle = 1;
            break;

        case VK_F1:
            if (f1Toggle)
                f1Toggle = 0;
            else
                f1Toggle = 1;
            break;
        }

        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_CHAR:
        if (line == MAX_LINE_NUMBER)
        {
            count = 0;
            line = 0;
        }

        if (keyDownBool)
        {
            if (wParam == VK_RETURN)
            {
                count = 0;
                ++line;
            }
            else if (wParam == VK_TAB)
            {
                for (int i = 0; i < 4; ++i)
                {
                    if (count != MAX_COUNT_NUMBER)
                        str[line][count++] = ' ';
                    else
                    {
                        str[line++][count] = '\0';
                        count = 0;
                        break;
                    }
                }
            }
            else
            {
                switch (insertToggle)
                {
                case 0:
                    switch (f1Toggle)
                    {
                    case 0:
                        str[line][count++] = wParam;
                        break;
                    case 1:
                        if (wParam >= 'a' && wParam <= 'z')
                            str[line][count++] = wParam - 32;
                        else
                            str[line][count++] = wParam;
                        break;
                    }
                    if (count == MAX_COUNT_NUMBER - 1)
                    {
                        count = 0;
                        ++line;
                    }
                    break;

                case 1:
                    int lineIdx = 0;
                    int lastIdx = lstrlen(str[line]);
                    while (str[line + lineIdx][lastIdx] != '\0')
                    {
                        if (lastIdx == (MAX_COUNT_NUMBER - 1))
                        {
                            ++lineIdx;
                            ++lastIdx;
                        }
                        ++lastIdx;
                    }
                    for (int i = lastIdx; i > count + 1; --i)
                    {
                        str[line + lineIdx][i] = str[line + lineIdx][i - 1];
                    }

                    switch (f1Toggle)
                    {
                    case 0:
                        str[line][count++] = wParam;
                        break;
                    case 1:
                        if (wParam >= 'a' && wParam <= 'z')
                            str[line][count++] = wParam - 32;
                        else
                            str[line][count++] = wParam;
                        break;
                    }

                    if (count == MAX_COUNT_NUMBER - 1)
                    {
                        count = 0;
                        ++line;
                    }
                    break;
                }
            }
        }

        keyDownBool = 1;

        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_PAINT:
        hDC = BeginPaint(hWnd, &ps);

        for (int i = 0; i < 10; ++i)
            TextOut(hDC, 0, i * 20, str[i], lstrlen(str[i]));

        GetTextExtentPoint32(hDC, str[line], count, &size);
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