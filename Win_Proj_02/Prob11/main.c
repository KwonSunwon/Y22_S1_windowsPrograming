#define _CRT_SECURE_NO_WARNINGS
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include <stdio.h>

#include <Windows.h>
#include <tchar.h>
#include <time.h>

#define GRID_LOCATION 20
#define GRID_SIZE 900

typedef struct _SHAPE
{
    int type;
    RECT location;
    COLORREF color[2];
    // int select;
} SHAPE;

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Window Programming Lab";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

// 리스트, type, squareCnt, listCnt
void newShape(SHAPE *, int, int, int);

// HDC, RECT - 그리드 가장자리 크기, int - 나눌 칸 갯수
void drawGrid(HDC, RECT, int);
void drawShape(HDC, SHAPE *, int, COLORREF, int);

void resetShape(SHAPE *);

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

    hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 980, 980,
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
    HDC hdc;

    HPEN hPen, oldPen;
    HBRUSH hBrush, oldBrush;

    static const RECT grid = {
        GRID_LOCATION,
        GRID_LOCATION,
        GRID_SIZE + GRID_LOCATION,
        GRID_SIZE + GRID_LOCATION,
    };

    static int squareCnt;
    static int squareSize;

    static SHAPE list[10];
    static int listCnt;
    static int listIdx;
    static int select;

    static int colorToggle;
    static COLORREF randColor[3];

    switch (iMessage)
    {
    case WM_CREATE:
        squareCnt = 30;
        squareSize = GRID_SIZE / squareCnt;
        listCnt = 0;
        listIdx = 0;
        select = 0;
        colorToggle = 0;
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case 0x53: // s
            squareCnt = 30;
            squareSize = GRID_SIZE / squareCnt;
            resetShape(list);
            listCnt = 0;
            listIdx = 0;
            select = 0;
            colorToggle = 0;
            break;
        case 0x4d: // m
            squareCnt = 40;
            squareSize = GRID_SIZE / squareCnt;
            resetShape(list);
            listCnt = 0;
            listIdx = 0;
            select = 0;
            colorToggle = 0;
            break;
        case 0x4c: // l
            squareCnt = 50;
            squareSize = GRID_SIZE / squareCnt;
            resetShape(list);
            listCnt = 0;
            listIdx = 0;
            select = 0;
            colorToggle = 0;
            break;

        case 0x45: // e 원 1
            if (listCnt < 10)
            {
                select = listCnt;
                newShape(list, 1, listCnt++, squareCnt);
            }
            else
            {
                for (int i = 0; i < 9; ++i)
                {
                    list[i] = list[i + 1];
                }
                listCnt = 9;
                select = listCnt;
                newShape(list, 1, listCnt++, squareCnt);
            }
            break;
        case 0x54: // t 삼각형 2
            if (listCnt != 10)
            {
                select = listCnt;
                newShape(list, 2, listCnt++, squareCnt);
            }
            else
            {
                for (int i = 0; i < 9; ++i)
                {
                    list[i] = list[i + 1];
                }
                listCnt = 9;
                select = listCnt;
                newShape(list, 2, listCnt++, squareCnt);
            }
            break;
        case 0x52: // r 사각형 3
            if (listCnt != 10)
            {
                select = listCnt;
                newShape(list, 3, listCnt++, squareCnt);
            }
            else
            {
                for (int i = 0; i < 9; ++i)
                {
                    list[i] = list[i + 1];
                }
                listCnt = 9;
                select = listCnt;
                newShape(list, 3, listCnt++, squareCnt);
            }
            break;

        case VK_UP:
            if (list[select].location.top <= GRID_LOCATION)
                break;
            else
                OffsetRect(&list[select].location, 0, -squareSize);
            break;
        case VK_LEFT:
            if (list[select].location.left <= GRID_LOCATION)
                break;
            else
                OffsetRect(&list[select].location, -squareSize, 0);
            break;
        case VK_DOWN:
            if (list[select].location.bottom >= GRID_LOCATION + GRID_SIZE)
                break;
            else
                OffsetRect(&list[select].location, 0, squareSize);
            break;
        case VK_RIGHT:
            if (list[select].location.right >= GRID_LOCATION + GRID_SIZE)
                break;
            else
                OffsetRect(&list[select].location, squareSize, 0);
            break;

        case VK_NUMPAD0:
            select = 0;
            break;
        case VK_NUMPAD1:
            select = 1;
            break;
        case VK_NUMPAD2:
            select = 2;
            break;
        case VK_NUMPAD3:
            select = 3;
            break;
        case VK_NUMPAD4:
            select = 4;
            break;
        case VK_NUMPAD5:
            select = 5;
            break;
        case VK_NUMPAD6:
            select = 6;
            break;
        case VK_NUMPAD7:
            select = 7;
            break;
        case VK_NUMPAD8:
            select = 8;
            break;
        case VK_NUMPAD9:
            select = 9;
            break;

        case VK_ADD:
            if (list[select].location.right >= GRID_LOCATION + GRID_SIZE || list[select].location.bottom >= GRID_LOCATION + GRID_SIZE)
                break;
            list[select].location.right += squareSize;
            list[select].location.bottom += squareSize;
            break;
        case VK_SUBTRACT:
            if (list[select].location.right - list[select].location.left == squareSize)
                break;
            list[select].location.right -= squareSize;
            list[select].location.bottom -= squareSize;
            break;

        case 0x43: // c 같은 도형끼리 같은 색으로 변경 (토글)
            if (colorToggle)
                colorToggle = 0;
            else
            {
                for (int i = 0; i < 3; ++i)
                    randColor[i] = RGB(rand() % 255, rand() % 255, rand() % 255);
                for (int i = 0; i < listCnt; ++i)
                {
                    switch (list[i].type)
                    {
                    case 1:
                        list[i].color[1] = randColor[0];
                        break;
                    case 2:
                        list[i].color[1] = randColor[1];
                        break;
                    case 3:
                        list[i].color[1] = randColor[2];
                        break;
                    }
                }
                colorToggle = 1;
            }
            break;

        case 0x44: // d 선택된 도형 삭제
            for (int i = select; i < listCnt - 1; ++i)
            {
                list[i] = list[i + 1];
            }
            list[listCnt--].type = 0;
            break;

        case 0x50: // p 리셋
            resetShape(list);
            listCnt = 0;
            listIdx = 0;
            select = 0;
            colorToggle = 0;
            break;

        case 0x51: // q 프로그램 종료
            PostQuitMessage(0);
            break;
        }

        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

        // 그리드 그리기
        drawGrid(hdc, grid, squareCnt);

        // 도형 그리기
        for (int i = 0; i < listCnt; ++i)
        {
            if (list[i].type == 0)
                continue;
            COLORREF rgb = BLACK_BRUSH;
            drawShape(hdc, &list, i, rgb, colorToggle);
        }

        // printf("%d", select);

        // 선택된 도형
        for (int i = 0; i < listCnt; ++i)
            if (select == i)
            {
                printf("선택된 도형\n");
                COLORREF rgb = RGB(255, 0, 0);
                drawShape(hdc, &list, i, rgb, colorToggle);
            }

        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:

        PostQuitMessage(0);
        return 0;
    }
    return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void drawGrid(HDC hdc, RECT grid, int size)
{
    HPEN hPen = CreatePen(PS_SOLID, 1, BLACK_BRUSH);
    float square = (grid.right - grid.left) / size;
    // printf("%d\n", square);
    FrameRect(hdc, &grid, hPen);
    for (int i = 1; i < size + 0.5; ++i)
    {
        MoveToEx(hdc, grid.left + square * i, grid.top, NULL);
        LineTo(hdc, grid.left + square * i, grid.bottom);
        MoveToEx(hdc, grid.left, grid.top + square * i, NULL);
        LineTo(hdc, grid.right, grid.top + square * i);
    }
}

void newShape(SHAPE *list, int type, int listCnt, int squareCnt)
{
    list[listCnt].type = type;
    // list[listCnt].select = 1;
    list[listCnt].location.left = GRID_LOCATION + ((GRID_SIZE / squareCnt) * (rand() % squareCnt));
    list[listCnt].location.top = GRID_LOCATION + ((GRID_SIZE / squareCnt) * (rand() % squareCnt));
    list[listCnt].location.right = list[listCnt].location.left + GRID_SIZE / squareCnt;
    list[listCnt].location.bottom = list[listCnt].location.top + GRID_SIZE / squareCnt;
    list[listCnt].color[0] = RGB(rand() % 255, rand() % 255, rand() % 255);
}

void resetShape(SHAPE *list)
{
    for (int i = 0; i < 10; ++i)
        list[i].type = 0;
}

void drawShape(HDC hdc, SHAPE *list, int i, COLORREF rgb, int colorToggle)
{
    HPEN hPen = CreatePen(PS_SOLID, 2, rgb);
    HPEN oldPen = (HPEN)SelectObject(hdc, hPen);

    HBRUSH hBrush = CreateSolidBrush(list[i].color[colorToggle]);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    switch (list[i].type)
    {
    case 1: // 원
        Ellipse(hdc, list[i].location.left, list[i].location.top, list[i].location.right, list[i].location.bottom);
        break;

    case 2: // 삼각형
        POINT temp[3] = {
            {(list[i].location.left + list[i].location.right) / 2, list[i].location.top},
            {list[i].location.left, list[i].location.bottom},
            {list[i].location.right, list[i].location.bottom},
        };
        Polygon(hdc, temp, 3);
        break;

    case 3: // 사각형
        Rectangle(hdc, list[i].location.left, list[i].location.top,
                  list[i].location.right, list[i].location.bottom);
        break;
    }

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);

    DeleteObject(hPen);
    DeleteObject(oldPen);
    DeleteObject(hBrush);
    DeleteObject(oldBrush);
}

// NOTE 선택된 도형 테두리 칠하기?, 선택된 번호의 도형을 그릴 때 확인하면 되지 않나?