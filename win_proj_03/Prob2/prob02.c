#define _CRT_SECURE_NO_WARNINGS
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include <stdio.h>

#include <Windows.h>
#include <tchar.h>

#define BALL_MOVE_TIME 1
#define BRICK_MOVE_TIME 2

typedef struct _BAR
{
    BOOL selection;
    RECT location;
} Bar;

typedef struct _BRICK
{
    RECT location;
    COLORREF color;
    BOOL isCrash;
} Brick;

typedef struct _BALL
{
    RECT location;
    int direction;
} Ball;

void drawBricks(HDC, Brick *);
void drawBall(HDC hdc, Ball ball);

void ballInit(Ball *, Bar);
void ballMove(Ball *);
int ballCrashWall(Ball *ball);
int ballCrashBrick(Ball *, Brick *);

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
    HDC hdc;
    PAINTSTRUCT ps;

    HPEN hPen, oldPen;
    HBRUSH hBrush, oldBrush;

    static Bar bar;
    static int ballMoveSpeed;

    static Brick bricks[30];

    static Ball ball;

    int mx, my;
    POINT mouse;

    BOOL gamePause;

    switch (iMessage)
    {
    case WM_CREATE:
        RECT barLocation = {350, 500, 450, 520};
        bar.location = barLocation;
        bar.selection = FALSE;

        int idx = 0;
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 10; ++j)
            {
                bricks[idx].isCrash = FALSE;
                bricks[idx].color = RGB(100, 100, 100);
                bricks[idx].location.left = 80 + j * 60;
                bricks[idx].location.top = 40 + i * 20;
                bricks[idx].location.right = bricks[idx].location.left + 60;
                bricks[idx].location.bottom = bricks[idx].location.top + 20;
                ++idx;
            }
        }

        ballInit(&ball, bar);

        gamePause = FALSE;

        ballMoveSpeed = 30;

        SetTimer(hWnd, BRICK_MOVE_TIME, 1000, NULL);
        break;

    case WM_TIMER:
        switch (wParam)
        {
        case BALL_MOVE_TIME:
            ballMove(&ball);
            if (ballCrashWall(&ball))
                ballInit(&ball, bar);
            ballCrashBrick(&ball, bricks);
            printf("%d\n", ball.direction);
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        }
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case 0x53: // S
            ballMoveSpeed = 30;
            SetTimer(hWnd, BALL_MOVE_TIME, ballMoveSpeed, NULL);
            break;

        case 0x50: // P
            break;

        case VK_ADD:
            KillTimer(hWnd, BALL_MOVE_TIME);
            ballMoveSpeed -= 5;
            SetTimer(hWnd, BALL_MOVE_TIME, ballMoveSpeed, NULL);
            break;

        case VK_SUBTRACT:
            KillTimer(hWnd, BALL_MOVE_TIME);
            ballMoveSpeed += 5;
            SetTimer(hWnd, BALL_MOVE_TIME, ballMoveSpeed, NULL);
            break;

        case 0x4E: // N
            break;

        case 0x51: // Q
            break;
        }

    case WM_LBUTTONDOWN:
        mx = LOWORD(lParam);
        my = HIWORD(lParam);

        mouse.x = mx;
        mouse.y = my;

        if (PtInRect(&bar.location, mouse))
        {
            bar.selection = TRUE;
        }
        break;

    case WM_MOUSEMOVE:
        if (bar.selection)
        {
            hdc = GetDC(hWnd);
            SetROP2(hdc, R2_XORPEN);
            SelectObject(hdc, (HPEN)GetStockObject(WHITE_PEN));
            SelectObject(hdc, (HBRUSH)GetStockObject(WHITE_BRUSH));

            mx = LOWORD(lParam);

            Rectangle(hdc, bar.location.left, bar.location.top, bar.location.right, bar.location.bottom);

            bar.location.left = mx - 50;
            bar.location.right = mx + 50;

            Rectangle(hdc, bar.location.left, bar.location.top, bar.location.right, bar.location.bottom);

            drawBricks(hdc, bricks);

            ReleaseDC(hWnd, hdc);
        }
        break;

    case WM_LBUTTONUP:
        bar.selection = FALSE;
        break;

    case WM_PAINT:
    {
        static HDC hdc, MemDC, tmpDC;
        static HBITMAP BackBit, oldBackBit;
        static RECT bufferRT;
        hdc = BeginPaint(hWnd, &ps);

        GetClientRect(hWnd, &bufferRT);
        MemDC = CreateCompatibleDC(hdc);
        BackBit = CreateCompatibleBitmap(hdc, bufferRT.right, bufferRT.bottom);
        oldBackBit = (HBITMAP)SelectObject(MemDC, BackBit);
        PatBlt(MemDC, 0, 0, bufferRT.right, bufferRT.bottom, WHITENESS);
        tmpDC = hdc;
        hdc = MemDC;
        MemDC = tmpDC;

        hBrush = CreateSolidBrush(BLACK_BRUSH);
        oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

        Rectangle(hdc, bar.location.left, bar.location.top, bar.location.right, bar.location.bottom);

        SelectObject(hdc, oldBrush);
        DeleteObject(hBrush);

        drawBricks(hdc, bricks);
        drawBall(hdc, ball);

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
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void drawBricks(HDC hdc, Brick *bricks)
{
    HBRUSH hBrush, oldBrush;
    for (int i = 0; i < 30; ++i)
    {
        hBrush = CreateSolidBrush(bricks[i].color);
        oldBrush = SelectObject(hdc, hBrush);
        Rectangle(hdc, bricks[i].location.left, bricks[i].location.top, bricks[i].location.right, bricks[i].location.bottom);
        SelectObject(hdc, oldBrush);
        DeleteObject(hBrush);
        // printf("%d, %d ,%d, %d\n", bricks[i].location.left, bricks[i].location.top, bricks[i].location.right, bricks[i].location.bottom);
    }
}

void drawBall(HDC hdc, Ball ball)
{
    HBRUSH hBrush, oldBrush;
    hBrush = CreateSolidBrush(BLACK_BRUSH);
    oldBrush = SelectObject(hdc, hBrush);
    Ellipse(hdc, ball.location.left, ball.location.top, ball.location.right, ball.location.bottom);
    SelectObject(hdc, oldBrush);
    DeleteObject(hBrush);

    // printf("ball : %d, %d ,%d, %d\n", ball.location.left, ball.location.top, ball.location.right, ball.location.bottom);
}

void ballMove(Ball *ball)
{
    switch (ball->direction)
    {
    case 0: // 왼쪽 위
        ball->location.left -= 5;
        ball->location.top -= 5;
        ball->location.right = ball->location.left + 20;
        ball->location.bottom = ball->location.top + 20;
        break;
    case 1: // 오른쪽 위
        ball->location.left += 5;
        ball->location.top -= 5;
        ball->location.right = ball->location.left + 20;
        ball->location.bottom = ball->location.top + 20;
        break;
    case 2: // 왼쪽 아래
        ball->location.left -= 5;
        ball->location.top += 5;
        ball->location.right = ball->location.left + 20;
        ball->location.bottom = ball->location.top + 20;
        break;
    case 3: // 오른쪽 아래
        ball->location.left += 5;
        ball->location.top += 5;
        ball->location.right = ball->location.left + 20;
        ball->location.bottom = ball->location.top + 20;
        break;
    }
}

int ballCrashWall(Ball *ball)
{
    if (ball->location.left <= 0 || ball->location.right >= 800)
    {
        if (ball->direction == 0)
            ball->direction = 1;
        else if (ball->direction == 1)
            ball->direction = 0;
        else if (ball->direction == 2)
            ball->direction = 3;
        else if (ball->direction == 3)
            ball->direction = 2;
    }
    else if (ball->location.top <= 0)
    {
        if (ball->direction == 0)
            ball->direction = 2;
        else if (ball->direction == 1)
            ball->direction = 3;
    }
    else if (ball->location.bottom >= 600)
    {
        return 1;
    }
    return 0;
}

void ballInit(Ball *ball, Bar bar)
{
    ball->location.left = (bar.location.left + bar.location.right) / 2 - 10;
    ball->location.top = bar.location.top - 20;
    ball->location.right = ball->location.left + 20;
    ball->location.bottom = bar.location.top;

    ball->direction = 0;
}

int ballCrashBrick(Ball *ball, Brick *bricks)
{
    RECT rcTemp;
    for (int i = 0; i < 30; ++i)
        if (IntersectRect(&rcTemp, &ball->location, &bricks[i].location) && !bricks[i].isCrash)
        {
            bricks[i].color = RGB(rand() % 255, rand() % 255, rand() % 255);
            bricks[i].isCrash = TRUE;
            if (ball->direction == 0)
            {
                if (ball->location.top <= bricks->location.bottom)
                    ball->direction = 3;
                else
                    ball->direction = 1;
            }
            else if (ball->direction == 1)
            {
                if (ball->location.top <= bricks->location.bottom)
                    ball->direction = 2;
                else
                    ball->direction = 0;
            }
            else if (ball->direction == 2)
            {
                if (ball->location.bottom >= bricks->location.top)
                    ball->direction = 0;
                else
                    ball->direction = 3;
            }
            else if (ball->direction == 3)
            {
                if (ball->location.bottom >= bricks->location.top)
                    ball->direction = 1;
                else
                    ball->direction = 2;
            }
        }
}