#define _CRT_SECURE_NO_WARNINGS
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include <stdio.h>

#include <Windows.h>
#include <tchar.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>

#include "resource.h"

#define CIRCLE_TIMER 1

#define RANDOM 0
#define RED 1
#define GREEN 2
#define BLUE 3

typedef struct _Circle
{
    POINT location;
    int radius;
    RECT sizeRT;
    COLORREF color;
    int rotation;
    int lineAngle;

    int angle;
    POINT subLocation;
    COLORREF subColor;
    int subRotation;
} Circle;

void initFirstCircle(Circle *);
POINT subCirclePt(int, int, POINT);
POINT linePt(int, int, POINT);

void drawCircle(HDC, Circle);
BOOL intersectCheck(Circle, Circle);

void circleMove(Circle *);

float lengthPT(int, int, int, int);
BOOL inCircle(int, int, int, int, int);

void makeCircle(Circle *, int, int);

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

    hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, (HMENU)NULL, hInstance, NULL);
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

    static HDC MemDC, tmpDC;
    static HBITMAP BackBit, oldBackBit;
    static RECT bufferRT;

    static Circle firstCircle[10];
    static int fIdx;
    static Circle circle[10];
    static int count;
    static int idx;
    static BOOL select;

    static BOOL isPause;
    static int colorSelect;

    int mx, my;

    switch (iMessage)
    {
    case WM_CREATE:
        initFirstCircle(firstCircle);
        count = 0;
        isPause = FALSE;
        fIdx = -1;
        idx = -1;
        select = FALSE;
        colorSelect = 0;
        SetTimer(hWnd, CIRCLE_TIMER, 50, NULL);
        break;

    case WM_TIMER:
        switch (wParam)
        {
        case CIRCLE_TIMER:
            if (!isPause)
            {
                for (int i = 0; i < 10; ++i)
                    circleMove(&firstCircle[i]);
                for (int i = 0; i < count; ++i)
                    circleMove(&circle[i]);
            }
            break;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_COMMAND:
        switch (wParam)
        {
        // 원 반대방향 자전
        case ID_MOVE_1:
            for (int i = 0; i < 10; ++i)
            {
                firstCircle[i].rotation = (firstCircle[i].rotation + 1) % 2;
            }
            for (int i = 0; i < 10; ++i)
            {
                circle[i].rotation = (circle[i].rotation + 1) % 2;
            }
            break;

        // 위성원 반대방향 공전
        case ID_MOVE_2:
            for (int i = 0; i < 10; ++i)
            {
                firstCircle[i].subRotation = (firstCircle[i].subRotation + 1) % 2;
            }
            for (int i = 0; i < 10; ++i)
            {
                circle[i].subRotation = (circle[i].subRotation + 1) % 2;
            }
            break;

        // 모든 원 정지, 다시회전
        case ID_MOVE_PAUSE:
            if (isPause)
            {
                SetTimer(hWnd, CIRCLE_TIMER, 50, NULL);
                isPause = FALSE;
            }
            else
            {
                KillTimer(hWnd, CIRCLE_TIMER);
                isPause = TRUE;
            }
            break;

        // 일부 원 색 반전
        case ID_CHANGE_INVERSION:
            for (int i = 0; i < 10; ++i)
                if (i % 2)
                {
                    COLORREF rgb = firstCircle[i].color;
                    firstCircle[i].color = RGB(255 - GetRValue(rgb), 255 - GetGValue(rgb), 255 - GetBValue(rgb));
                }
            for (int i = 0; i < count; ++i)
                if (i % 2)
                {
                    COLORREF rgb = circle[i].color;
                    circle[i].color = RGB(255 - GetRValue(rgb), 255 - GetGValue(rgb), 255 - GetBValue(rgb));
                }
            break;

        // 원 색깔 빨강, 초록, 파랑, 랜덤 설정
        case ID_CHANGE_RED:
            colorSelect = RED;
            break;
        case ID_CHANGE_GREEN:
            colorSelect = GREEN;
            break;
        case ID_CHANGE_BLUE:
            colorSelect = BLUE;
            break;
        case ID_CHANGE_RANDOM:
            colorSelect = RANDOM;
            break;

        // 새로운 게임 시작
        case ID_GAME_RESET:
            initFirstCircle(firstCircle);
            count = 0;
            isPause = FALSE;
            fIdx = -1;
            idx = -1;
            select = FALSE;
            colorSelect = 0;
            SetTimer(hWnd, CIRCLE_TIMER, 50, NULL);
            break;

        // 프로그램 종료
        case ID_GAME_QUIT:
            PostQuitMessage(0);
            return 0;
        }
        break;

    // 원 내부 : 위성원 공전 방향 변경
    // 원이 없는 공간 : 새로운 원 생성(최대 10개)
    case WM_LBUTTONDOWN:
        mx = LOWORD(lParam);
        my = HIWORD(lParam);

        for (int i = 0; i < 10; ++i)
            if (inCircle(mx, my, firstCircle[i].location.x, firstCircle[i].location.y, firstCircle[i].radius))
            {
                fIdx = i;
                select = TRUE;
            }
        if (!select)
            for (int i = 0; i < count; ++i)
            {
                if (inCircle(mx, my, circle[i].location.x, circle[i].location.y, circle[i].radius))
                {
                    idx = i;
                    select = TRUE;
                }
            }

        // 공전 방향 변경
        if (select)
        {
            if (fIdx != -1)
                firstCircle[fIdx].subRotation = (firstCircle[fIdx].subRotation + 1) % 2;
            else if (idx != -1)
                circle[idx].subRotation = (circle[idx].subRotation + 1) % 2;
        }
        // 새로운 원 생성
        else
        {
            if (count == 10)
                break;
            makeCircle(&circle[count], mx, my);
            if (colorSelect)
            {
                switch (colorSelect)
                {
                case RED:
                    circle[count].color = RGB(255, 0, 0);
                    break;
                case GREEN:
                    circle[count].color = RGB(0, 255, 0);
                    break;
                case BLUE:
                    circle[count].color = RGB(0, 0, 255);
                    break;
                }
            }
            ++count;
        }

        fIdx = -1;
        idx = -1;
        select = FALSE;

        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &bufferRT);
        MemDC = CreateCompatibleDC(hdc);
        BackBit = CreateCompatibleBitmap(hdc, bufferRT.right, bufferRT.bottom);
        oldBackBit = (HBITMAP)SelectObject(MemDC, BackBit);
        PatBlt(MemDC, 0, 0, bufferRT.right, bufferRT.bottom, WHITENESS);
        tmpDC = hdc;
        hdc = MemDC;
        MemDC = tmpDC;

        //그리기
        for (int i = 0; i < 10; ++i)
            drawCircle(hdc, firstCircle[i]);
        for (int i = 0; i < count; ++i)
            drawCircle(hdc, circle[i]);

        tmpDC = hdc;
        hdc = MemDC;
        MemDC = tmpDC;
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

void initFirstCircle(Circle *circle)
{
    srand(time(NULL));

    for (int i = 0; i < 10; ++i)
    {
        circle[i].location.x = rand() % 601 + 100;
        circle[i].location.y = rand() % 401 + 100;
        circle[i].radius = rand() % 41 + 10;
        circle[i].color = RGB(rand() % 256, rand() % 256, rand() % 256);
        circle[i].rotation = rand() % 2;

        circle[i].sizeRT.left = circle[i].location.x - circle[i].radius;
        circle[i].sizeRT.top = circle[i].location.y - circle[i].radius;
        circle[i].sizeRT.right = circle[i].location.x + circle[i].radius;
        circle[i].sizeRT.bottom = circle[i].location.y + circle[i].radius;

        if (i != 0)
            for (int j = 0; j < i; ++j)
                if (intersectCheck(circle[i], circle[j]))
                {
                    --i;
                    continue;
                }

        circle[i].subColor = RGB(rand() % 256, rand() % 256, rand() % 256);
        circle[i].subRotation = rand() % 2;
        circle[i].angle = rand() % 361;
        circle[i].lineAngle = rand() % 361;

        circle[i].subLocation = subCirclePt(circle[i].angle, circle[i].radius, circle[i].location);
    }
}

POINT subCirclePt(int angle, int radius, POINT location)
{
    return (POINT){
        location.x + (radius + 5) * cos(angle * M_PI / 180),
        location.y + (radius + 5) * sin(angle * M_PI / 180),
    };
}

POINT linePt(int angle, int radius, POINT location)
{
    return (POINT){
        location.x + radius * cos(angle * M_PI / 180),
        location.y + radius * sin(angle * M_PI / 180),
    };
}

void drawCircle(HDC hdc, Circle circle)
{
    HBRUSH hBrush, oldBrush;

    hBrush = CreateSolidBrush(circle.color);
    oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    Ellipse(hdc, circle.sizeRT.left, circle.sizeRT.top, circle.sizeRT.right, circle.sizeRT.bottom);
    SelectObject(hdc, oldBrush);
    DeleteObject(hBrush);

    RECT subRT = {
        circle.subLocation.x - 5,
        circle.subLocation.y - 5,
        circle.subLocation.x + 5,
        circle.subLocation.y + 5,
    };

    hBrush = CreateSolidBrush(circle.subColor);
    oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    Ellipse(hdc, subRT.left, subRT.top, subRT.right, subRT.bottom);
    SelectObject(hdc, oldBrush);
    DeleteObject(hBrush);

    POINT line;
    int angle = circle.lineAngle;
    for (int i = 0; i < 8; ++i)
    {
        line = linePt(angle, circle.radius, circle.location);
        angle += 45;
        MoveToEx(hdc, circle.location.x, circle.location.y, NULL);
        LineTo(hdc, line.x, line.y);
    }
}

BOOL intersectCheck(Circle c1, Circle c2)
{
    RECT rcTmp;
    if (IntersectRect(&rcTmp, &c1.sizeRT, &c2.sizeRT))
        return TRUE;
    else
        return FALSE;
}

void circleMove(Circle *c)
{
    if (c->rotation)
        c->lineAngle = (c->lineAngle + 5) % 360;
    else
        c->lineAngle = (c->lineAngle - 5) % 360;

    if (c->subRotation)
        c->angle = (c->angle + 5) % 360;
    else
        c->angle = (c->angle - 5) % 360;

    c->subLocation = subCirclePt(c->angle, c->radius, c->location);
}

float lengthPT(int x1, int y1, int x2, int y2)
{
    return (sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)));
}

BOOL inCircle(int x1, int y1, int x2, int y2, int r)
{
    if (lengthPT(x1, y1, x2, y2) < r)
        return TRUE;
    else
        return FALSE;
}

void makeCircle(Circle *circle, int x, int y)
{
    circle->location.x = x;
    circle->location.y = y;
    circle->radius = rand() % 41 + 10;
    circle->color = RGB(rand() % 256, rand() % 256, rand() % 256);
    circle->rotation = rand() % 2;

    circle->sizeRT.left = circle->location.x - circle->radius;
    circle->sizeRT.top = circle->location.y - circle->radius;
    circle->sizeRT.right = circle->location.x + circle->radius;
    circle->sizeRT.bottom = circle->location.y + circle->radius;

    circle->subColor = RGB(rand() % 256, rand() % 256, rand() % 256);
    circle->subRotation = rand() % 2;
    circle->angle = rand() % 361;
    circle->lineAngle = rand() % 361;

    circle->subLocation = subCirclePt(circle->angle, circle->radius, circle->location);
}