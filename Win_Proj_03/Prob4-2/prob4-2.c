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
void drawShape(HDC, Shape *, int, BOOL);
void drawIntersectShape(HDC, Shape *, int);

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
    // static RECT grid = {30, 30, 780, 780};
    static RECT grid;
    static int line;

    // Shape
    static Shape shape[MAX_COUNT];
    static int count;
    static int idx;

    static BOOL lClick, rClick;
    static COLORREF selectColor;
    static BOOL border;

    static RECT newShapeDraw;

    static RECT moveShape;
    static POINT move;

    long mx, my;
    POINT mouse;
    RECT rcTemp;

    switch (iMessage)
    {
    case WM_CREATE:
        GetClientRect(hWnd, &rcTemp);
        grid.left = rcTemp.left + 30;
        grid.top = rcTemp.top + 30;
        grid.right = grid.left + 750;
        grid.bottom = grid.top + 750;

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
                newShapeDraw = shape[i].location;
                moveShape.left = mx - newShapeDraw.left;
                moveShape.top = my - newShapeDraw.top;
                moveShape.right = newShapeDraw.right - mx;
                moveShape.bottom = newShapeDraw.bottom - my;

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
            newShapeDraw.left = mx - moveShape.left;
            newShapeDraw.top = my - moveShape.top;
            newShapeDraw.right = mx + moveShape.right;
            newShapeDraw.bottom = my + moveShape.bottom;

            shape[idx].location = newShapeDraw;

            InvalidateRect(hWnd, NULL, FALSE);
        }
        ReleaseDC(hWnd, hdc);
        break;

    // 도형 그리기 완료
    case WM_LBUTTONUP:
        lClick = FALSE;
        newShapeDraw.right = newShapeDraw.right / 30 * 30 + 30;
        newShapeDraw.bottom = newShapeDraw.bottom / 30 * 30 + 30;
        shape[count].location = newShapeDraw;
        shape[count++].color = selectColor;
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    // 도형 이동 완료
    case WM_RBUTTONUP:
        rClick = FALSE;

        newShapeDraw.left = newShapeDraw.left / 30 * 30;
        newShapeDraw.top = newShapeDraw.top / 30 * 30;
        newShapeDraw.right = newShapeDraw.right / 30 * 30;
        newShapeDraw.bottom = newShapeDraw.bottom / 30 * 30;

        shape[idx].location = newShapeDraw;
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_COMMAND:
        switch (wParam)
        {
        // 내부 grid 실선, 점선 변경 1. 실선 2. 점선
        case ID_GRID_SOLID:
            line = 1;
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        case ID_GRID_DOTTED:
            line = 2;
            InvalidateRect(hWnd, NULL, FALSE);
            break;

        // 색상 변경
        case ID_COLOR_RED:
            selectColor = RGB(255, 0, 0);
            break;
        case ID_COLOR_GREEN:
            selectColor = RGB(0, 255, 0);
            break;
        case ID_COLOR_BLUE:
            selectColor = RGB(0, 0, 255);
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
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        case ID_BORDER_OFF:
            border = FALSE;
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        }
        break;

    case WM_PAINT:
        // 더블버퍼링 정의
        static HDC MemDC, tmpDC;
        static HBITMAP BackBit, oldBackBit;
        static RECT bufferRT;

        hdc = BeginPaint(hWnd, &ps);

        // 더블버퍼링
        GetClientRect(hWnd, &bufferRT);
        MemDC = CreateCompatibleDC(hdc);
        BackBit = CreateCompatibleBitmap(hdc, bufferRT.right, bufferRT.bottom);
        oldBackBit = (HBITMAP)SelectObject(MemDC, BackBit);
        PatBlt(MemDC, 0, 0, bufferRT.right, bufferRT.bottom, WHITENESS);
        // tmpDC = hdc;
        // hdc = MemDC;
        // MemDC = tmpDC;

        // 그리기
        drawGrid(MemDC, grid, 25, line);
        drawShape(MemDC, shape, count, border);
        drawIntersectShape(MemDC, shape, count);

        // 더블버퍼링
        // tmpDC = hdc;
        // hdc = MemDC;
        // MemDC = tmpDC;
        GetClientRect(hWnd, &bufferRT);
        BitBlt(hdc, 0, 0, bufferRT.right, bufferRT.bottom, MemDC, 0, 0, SRCCOPY);
        SelectObject(MemDC, oldBackBit);
        DeleteObject(BackBit);
        DeleteDC(MemDC);

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

void drawShape(HDC hdc, Shape *shape, int count, BOOL border)
{
    HBRUSH hBrush, oldBrush;
    HPEN hPen, oldPen;

    if (border)
        hPen = CreatePen(PS_SOLID, 3, BLACK_PEN);
    else
        hPen = CreatePen(PS_NULL, 0, NULL);

    oldPen = (HPEN)SelectObject(hdc, hPen);

    for (int i = 0; i < count; ++i)
    {
        hBrush = CreateSolidBrush(shape[i].color);
        oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

        RECT rect = shape[i].location;
        Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);

        SelectObject(hdc, oldBrush);
        DeleteObject(hBrush);
    }

    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
}

void drawIntersectShape(HDC hdc, Shape *shape, int count)
{
    HBRUSH hBrush, oldBrush;
    HPEN hPen, oldPen;

    Shape interShape;

    hPen = CreatePen(PS_NULL, 0, NULL);
    oldPen = (HPEN)SelectObject(hdc, hPen);

    for (int i = 0; i < count; ++i)
        for (int j = 0; j < count; ++j)
            if (i != j && IntersectRect(&interShape.location, &shape[i].location, &shape[j].location))
            {
                interShape.color = shape[i].color + shape[j].color;
                hBrush = CreateSolidBrush(interShape.color);
                oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

                RECT rect = interShape.location;
                Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);

                SelectObject(hdc, oldBrush);
                DeleteObject(hBrush);
            }

    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
}