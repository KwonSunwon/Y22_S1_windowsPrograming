#define _CRT_SECURE_NO_WARNINGS
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include <stdio.h>

#include <Windows.h>
#include <tchar.h>

#include "resource.h"

#define MAX_COUNT 50

typedef struct _Shape
{
    RECT location;
    COLORREF color;
} Shape;

void drawGrid(HDC, RECT, int, int);
void drawShape(HDC, Shape *, int);

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
    WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
    WndClass.lpszClassName = lpszClass;
    WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassEx(&WndClass);

    hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 850, 880, NULL, (HMENU)NULL, hInstance, NULL);
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
    HDC hdc;

    // Grid
    static RECT grid = {30, 30, 780, 780};
    static int line;

    // Shape
    static Shape shape[MAX_COUNT];
    static int count;
    static int idx;

    static BOOL lClick, rClick;
    static COLORREF selectColor;
    static BOOL border;

    static RECT newShapeDraw;

    int mx, my;
    POINT mouse;

    switch (iMessage)
    {
    case WM_CREATE:
        line = 1;
        border = TRUE;
        lClick = FALSE;
        rClick = FALSE;
        count = 0;
        idx = -1;
        selectColor = RGB(255, 0, 0);
        break;

    // 도형 그리기 시작
    case WM_LBUTTONDOWN:
        mx = LOWORD(lParam);
        my = HIWORD(lParam);
        if (count < MAX_COUNT)
        {
            lClick = TRUE;
            newShapeDraw.left = mx / 30 * 30;
            newShapeDraw.top = my / 30 * 30;
            newShapeDraw.right = newShapeDraw.left;
            newShapeDraw.bottom = newShapeDraw.top;
            // printf("%d, %d, %d, %d\n", newShapeDraw.left, newShapeDraw.top, newShapeDraw.right, newShapeDraw.bottom);
        }
        break;
    // 도형 이동 시작
    case WM_RBUTTONDOWN:
        mx = LOWORD(lParam);
        my = HIWORD(lParam);
        mouse.x = mx;
        mouse.y = my;

        for (int i = 0; i < count; ++i)
        {
            if (PtInRect(&shape[i].location, mouse))
            {
                rClick = TRUE;
                idx = i;
                break;
            }
        }
        break;

    case WM_MOUSEMOVE:
        mx = LOWORD(lParam);
        my = HIWORD(lParam);

        hdc = GetDC(hWnd);
        if (lClick)
        {
            SetROP2(hdc, R2_XORPEN);
            SelectObject(hdc, (HPEN)GetStockObject(WHITE_PEN));
            SelectObject(hdc, (HBRUSH)GetStockObject(BLACK_BRUSH));

            Rectangle(hdc, newShapeDraw.left, newShapeDraw.top, newShapeDraw.right, newShapeDraw.bottom);

            newShapeDraw.right = mx;
            newShapeDraw.bottom = my;

            Rectangle(hdc, newShapeDraw.left, newShapeDraw.top, newShapeDraw.right, newShapeDraw.bottom);
        }
        else if (rClick)
        {
        }
        ReleaseDC(hWnd, hdc);
        break;

    // 도형 그리기 완료
    case WM_LBUTTONUP:
        lClick = FALSE;
        newShapeDraw.right = newShapeDraw.right / 30 * 30;
        newShapeDraw.bottom = newShapeDraw.bottom / 30 * 30;
        shape[count].location = newShapeDraw;
        shape[count++].color = selectColor;
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    // 도형 이동 완료
    case WM_RBUTTONUP:
        rClick = FALSE;
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_MENUCOMMAND:
        switch (wParam)
        {
        // 내부 grid 실선, 점선 변경 1. 실선 2. 점선
        case ID_GRID_SOLID:
            line = 1;
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        case ID_GRID_DOTTED:
            line = 2;
            InvalidateRect(hWnd, NULL, TRUE);
            break;

        // 색상 변경
        case ID_COLOR_RED:
            selectColor = RGB(255, 50, 50);
            break;
        case ID_COLOR_GREEN:
            selectColor = RGB(50, 255, 50);
            break;
        case ID_COLOR_BLUE:
            selectColor = RGB(50, 50, 255);
            break;
        case ID_COLOR_CYAN:
            selectColor = RGB(0, 255, 255);
            break;
        case ID_COLOR_MAGENTA:
            selectColor = RGB(255, 0, 255);
            break;
        case ID_COLOR_YELLOW:
            selectColor = RGB(255, 255, 0);
            break;

        // 도형 테두리
        case ID_BORDER_ON:
            border = TRUE;
            break;
        case ID_BORDER_OFF:
            border = FALSE;
            break;
        }
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

        drawGrid(hdc, grid, 30, line);
        drawShape(hdc, shape, count);
        printf("%d, %d, %d, %d\n", newShapeDraw.left, newShapeDraw.top, newShapeDraw.right, newShapeDraw.bottom);

        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void drawGrid(HDC hdc, RECT grid, int size, int line)
{
    HPEN hPen, oldPen;
    if (line == 1)
        hPen = CreatePen(PS_SOLID, 1, BLACK_BRUSH);
    else
        hPen = CreatePen(PS_DOT, 1, BLACK_BRUSH);

    oldPen = (HPEN)SelectObject(hdc, hPen);

    int square = (grid.right - grid.left) / size;
    FrameRect(hdc, &grid, hPen);
    for (int i = 1; i < size; ++i)
    {
        MoveToEx(hdc, grid.left + square * i, grid.top, NULL);
        LineTo(hdc, grid.left + square * i, grid.bottom);
        MoveToEx(hdc, grid.left, grid.top + square * i, NULL);
        LineTo(hdc, grid.right, grid.top + square * i);
    }

    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
}

void drawShape(HDC hdc, Shape *shape, int count)
{
    HBRUSH hBrush, oldBrush;
    for (int i = 0; i < count; ++i)
    {
        hBrush = CreateSolidBrush(shape[i].color);
        oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

        RECT rect = shape[i].location;
        Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);

        SelectObject(hdc, oldBrush);
        DeleteObject(hBrush);
    }
}