#define _CRT_SECURE_NO_WARNINGS
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include <stdio.h>

#include <Windows.h>
#include <tchar.h>

#define BALL_MOVE_TIME 1
#define BRICK_MOVE_TIME 2
#define BRICK_DISAPPEAR_TIME 3

#define LEFT_UP 0
#define RIGHT_UP 1
#define LEFT_DOWN 2
#define RIGHT_DOWN 3

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
    BOOL isDelete;
    int direction;
    int right;
    int left;
} Brick;

typedef struct _BALL
{
    RECT location;
    int direction;
} Ball;

// Ball
void ballInit(Ball *, Bar); // ���� �������� �ǵ��� ���� �Ʒ� ������ ������ �� ����
void drawBall(HDC, Ball *);
void ballMove(Ball *); // �� �̵�

// Brick
void brickInit(Brick *);
void drawBricks(HDC, Brick *);
void bricksMove(Brick *);
BOOL brickDisappear(Brick *); //�縮���� �ִ� ���� ������ TRUE

// Bar
void barInit(Bar *);
void drawBar(HDC, Bar *);

// �浹ó��
int ballCrashWall(Ball *);           // ��(ȭ��)�� �⵿ Ȯ��, �Ʒ������� ������ ��� TRUE return
int ballCrashBrick(Ball *, Brick *); // ������ �浹 Ȯ��
void ballCrashBar(Ball *, Bar *);

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
    HFONT hFont, oldFont;

    static Bar bar;
    static int ballMoveSpeed;
    static int bricksMoveSpeed;

    static Brick bricks[30];
    static Ball ball;

    static BOOL isGameStart;
    static BOOL gamePause;

    int mx, my;
    POINT mouse;

    static TCHAR pauseMassage[100];
    static RECT textBox = {330, 300, 450, 400};
    int coloured;
    int disappeared;

    switch (iMessage)
    {
    case WM_CREATE:

        barInit(&bar);
        brickInit(bricks);
        ballInit(&ball, bar);

        isGameStart = FALSE;
        gamePause = FALSE;
        ballMoveSpeed = 20;
        bricksMoveSpeed = 500;

        break;

    case WM_TIMER:
        switch (wParam)
        {
        case BALL_MOVE_TIME:
            ballMove(&ball);
            if (ballCrashWall(&ball))
                ballInit(&ball, bar);
            ballCrashBrick(&ball, bricks);
            ballCrashBar(&ball, &bar);
            // printf("���� ���� %d\n", ball.direction);
            break;

        case BRICK_MOVE_TIME:
            bricksMove(bricks);
            if (brickDisappear(bricks))
                SetTimer(hWnd, BRICK_DISAPPEAR_TIME, 10, NULL);
            else
                KillTimer(hWnd, BRICK_DISAPPEAR_TIME);
            break;

        case BRICK_DISAPPEAR_TIME:
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        // S : �� Ƣ��� ����
        case 0x53:
            if (isGameStart)
                break;
            isGameStart = TRUE;
            ballMoveSpeed = 20;
            SetTimer(hWnd, BALL_MOVE_TIME, ballMoveSpeed, NULL);
            SetTimer(hWnd, BRICK_MOVE_TIME, bricksMoveSpeed, NULL);
            break;

        //�Ͻ����� �� ȭ�鿡 ���� ���� ���� ������ ����� ���� ���� ���
        // P : �Ͻ�����/����
        case 0x50:
            if (isGameStart)
                if (gamePause)
                {
                    gamePause = FALSE;
                    SetTimer(hWnd, BALL_MOVE_TIME, ballMoveSpeed, NULL);
                    SetTimer(hWnd, BRICK_MOVE_TIME, bricksMoveSpeed, NULL);
                }
                else
                {
                    coloured = 0;
                    disappeared = 0;
                    for (int i = 0; i < 30; ++i)
                    {
                        if (bricks[i].isCrash)
                            coloured++;
                        else if (bricks[i].isDelete)
                            disappeared++;
                    }
                    wsprintf(pauseMassage, L"Coloured : %d\nDisappeared : %d", coloured, disappeared);

                    gamePause = TRUE;
                    KillTimer(hWnd, BALL_MOVE_TIME);
                    KillTimer(hWnd, BRICK_MOVE_TIME);
                }
            InvalidateRect(hWnd, NULL, FALSE);
            break;

        // �� �ӵ� ����
        case VK_ADD:
            if (ballMoveSpeed == 0 || !isGameStart)
                break;
            KillTimer(hWnd, BALL_MOVE_TIME);
            ballMoveSpeed -= 2;
            SetTimer(hWnd, BALL_MOVE_TIME, ballMoveSpeed, NULL);
            break;

        // �� �ӵ� ����
        case VK_SUBTRACT:
            if (!isGameStart)
                break;
            KillTimer(hWnd, BALL_MOVE_TIME);
            ballMoveSpeed += 2;
            SetTimer(hWnd, BALL_MOVE_TIME, ballMoveSpeed, NULL);
            break;

        // N : ���� ����
        case 0x4E:
            KillTimer(hWnd, BALL_MOVE_TIME);
            KillTimer(hWnd, BRICK_MOVE_TIME);

            barInit(&bar);
            brickInit(bricks);
            ballInit(&ball, bar);

            isGameStart = FALSE;
            gamePause = FALSE;
            ballMoveSpeed = 20;
            SetTimer(hWnd, BRICK_MOVE_TIME, bricksMoveSpeed, NULL);
            break;

        // Q : ���α׷� ����
        case 0x51:
            KillTimer(hWnd, BALL_MOVE_TIME);
            KillTimer(hWnd, BRICK_MOVE_TIME);
            PostQuitMessage(0);
            return 0;
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
        // bar �� ���� �� ����, ������ �������� ���� ����
        if (bar.selection && !gamePause)
        {
            mx = LOWORD(lParam);

            bar.location.left = mx - 50;
            bar.location.right = mx + 50;

            if (!isGameStart)
                ballInit(&ball, bar);

            InvalidateRect(hWnd, NULL, FALSE);
        }
        break;
    case WM_LBUTTONUP:
        bar.selection = FALSE;
        break;

    case WM_PAINT:
    {
        // ������۸� ����
        static HDC MemDC, tmpDC;
        static HBITMAP BackBit, oldBackBit;
        static RECT bufferRT;

        hdc = BeginPaint(hWnd, &ps);

        // ������۸�
        GetClientRect(hWnd, &bufferRT);
        MemDC = CreateCompatibleDC(hdc);
        BackBit = CreateCompatibleBitmap(hdc, bufferRT.right, bufferRT.bottom);
        oldBackBit = (HBITMAP)SelectObject(MemDC, BackBit);
        PatBlt(MemDC, 0, 0, bufferRT.right, bufferRT.bottom, WHITENESS);
        tmpDC = hdc;
        hdc = MemDC;
        MemDC = tmpDC;

        // �׸���
        hBrush = CreateSolidBrush(BLACK_BRUSH);
        oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

        drawBar(hdc, &bar);

        SelectObject(hdc, oldBrush);
        DeleteObject(hBrush);

        drawBricks(hdc, bricks);
        drawBall(hdc, &ball);

        if (gamePause)
        {
            hFont = CreateFont(20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            oldFont = (HFONT)SelectObject(hdc, hFont);
            SetTextColor(hdc, RGB(0, 0, 0));
            SetBkColor(hdc, RGB(255, 255, 255));
            DrawText(hdc, pauseMassage, lstrlen(pauseMassage), &textBox, NULL);
            SelectObject(hdc, oldFont);
            DeleteObject(hFont);
        }

        // ������۸�
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

    // ������ ������ ����
    case WM_GETMINMAXINFO:
        ((LPMINMAXINFO)lParam)->ptMinTrackSize.x = 800;
        ((LPMINMAXINFO)lParam)->ptMinTrackSize.y = 600;
        ((LPMINMAXINFO)lParam)->ptMaxTrackSize.x = 800;
        ((LPMINMAXINFO)lParam)->ptMaxTrackSize.y = 600;
        break;

    case WM_DESTROY:
        KillTimer(hWnd, BALL_MOVE_TIME);
        KillTimer(hWnd, BRICK_MOVE_TIME);
        PostQuitMessage(0);
        return 0;
    }
    return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

// Ball
void ballInit(Ball *ball, Bar bar)
{
    ball->location.left = (bar.location.left + bar.location.right) / 2 - 10;
    ball->location.top = bar.location.top - 20;
    ball->location.right = ball->location.left + 20;
    ball->location.bottom = bar.location.top;

    ball->direction = LEFT_UP;
}

void drawBall(HDC hdc, Ball *ball)
{
    HBRUSH hBrush, oldBrush;
    HPEN hPen, oldPen;

    hBrush = CreateSolidBrush(RGB(50, 100, 50));
    oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    hPen = CreatePen(PS_SOLID, 1, RGB(50, 100, 50));
    oldPen = (HPEN)SelectObject(hdc, hPen);

    Ellipse(hdc, ball->location.left, ball->location.top, ball->location.right, ball->location.bottom);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(hBrush);
    DeleteObject(hPen);
}

void ballMove(Ball *ball)
{
    switch (ball->direction)
    {
    case LEFT_UP: // ���� ��
        ball->location.left -= 3;
        ball->location.top -= 3;
        ball->location.right = ball->location.left + 20;
        ball->location.bottom = ball->location.top + 20;
        break;
    case RIGHT_UP: // ������ ��
        ball->location.left += 3;
        ball->location.top -= 3;
        ball->location.right = ball->location.left + 20;
        ball->location.bottom = ball->location.top + 20;
        break;
    case RIGHT_DOWN: // ���� �Ʒ�
        ball->location.left += 3;
        ball->location.top += 3;
        ball->location.right = ball->location.left + 20;
        ball->location.bottom = ball->location.top + 20;
        break;
    case LEFT_DOWN: // ������ �Ʒ�
        ball->location.left -= 3;
        ball->location.top += 3;
        ball->location.right = ball->location.left + 20;
        ball->location.bottom = ball->location.top + 20;
        break;
    }
}

// Brick
void brickInit(Brick *bricks)
{
    int idx = 0;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 10; ++j)
        {
            bricks[idx].isCrash = FALSE;
            bricks[idx].isDelete = FALSE;
            bricks[idx].color = RGB(100, 100, 100);
            bricks[idx].location.left = 80 + j * 60;
            bricks[idx].location.top = 40 + i * 20;
            bricks[idx].location.right = bricks[idx].location.left + 60;
            bricks[idx].location.bottom = bricks[idx].location.top + 20;
            ++idx;
        }
    }
    bricks[0].direction = 1;
    bricks[0].left = 80;
    bricks[0].right = 680;
}

void drawBricks(HDC hdc, Brick *bricks)
{
    HBRUSH hBrush, oldBrush;
    HPEN hPen, oldPen;

    hPen = CreatePen(PS_NULL, 1, WHITE_PEN);
    oldPen = (HPEN)SelectObject(hdc, hPen);

    for (int i = 0; i < 30; ++i)
    {
        if (bricks[i].isDelete)
            continue;
        hBrush = CreateSolidBrush(bricks[i].color);
        oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
        Rectangle(hdc, bricks[i].location.left, bricks[i].location.top, bricks[i].location.right, bricks[i].location.bottom);
        SelectObject(hdc, oldBrush);
        DeleteObject(hBrush);
    }

    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
}

void bricksMove(Brick *bricks)
{
    if (bricks[0].left <= 50)
        bricks[0].direction = 1;
    else if (bricks[0].right >= 750)
        bricks[0].direction = 0;

    if (bricks[0].direction == 1)
    {
        for (int i = 0; i < 30; ++i)
        {
            bricks[i].location.left += 10;
            bricks[i].location.right += 10;
        }
        bricks[0].left += 10;
        bricks[0].right += 10;
    }
    else
    {
        for (int i = 0; i < 30; ++i)
        {
            bricks[i].location.left -= 10;
            bricks[i].location.right -= 10;
        }
        bricks[0].left -= 10;
        bricks[0].right -= 10;
    }
}

BOOL brickDisappear(Brick *bricks)
{
    BOOL isExist = FALSE;
    for (int i = 0; i < 30; ++i)
    {
        if (bricks[i].isCrash)
        {
            if (bricks[i].location.left == bricks[i].location.right && bricks[i].location.top == bricks[i].location.bottom)
            {
                bricks[i].isDelete = TRUE;
                bricks[i].isCrash = FALSE;
                continue;
            }
            InflateRect(&bricks[i].location, -3, -1);
            isExist = TRUE;
        }
    }
    return isExist;
}

// Bar
void barInit(Bar *bar)
{
    RECT barLocation = {350, 500, 450, 520};
    bar->location = barLocation;
    bar->selection = FALSE;
}

void drawBar(HDC hdc, Bar *bar)
{
    HBRUSH hBrush, oldBrush;
    hBrush = CreateSolidBrush(RGB(50, 100, 50));
    oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    Rectangle(hdc, bar->location.left, bar->location.top, bar->location.right, bar->location.bottom);

    SelectObject(hdc, oldBrush);
    DeleteObject(hBrush);
}

// �浹ó��
int ballCrashWall(Ball *ball)
{
    if (ball->location.left <= 0 || ball->location.right >= 800)
    {
        if (ball->direction == LEFT_UP)
            ball->direction = RIGHT_UP;
        else if (ball->direction == RIGHT_UP)
            ball->direction = LEFT_UP;
        else if (ball->direction == RIGHT_DOWN)
            ball->direction = LEFT_DOWN;
        else if (ball->direction == LEFT_DOWN)
            ball->direction = RIGHT_DOWN;
    }
    else if (ball->location.top <= 0)
    {
        if (ball->direction == LEFT_UP)
            ball->direction = LEFT_DOWN;
        else if (ball->direction == RIGHT_UP)
            ball->direction = RIGHT_DOWN;
    }
    else if (ball->location.bottom >= 600)
    {
        return 1;
    }
    return 0;
}

int ballCrashBrick(Ball *ball, Brick *bricks)
{
    RECT rcTemp;
    for (int i = 0; i < 30; ++i)
        if (IntersectRect(&rcTemp, &ball->location, &bricks[i].location) && !bricks[i].isCrash)
        {
            POINT brickCenter = {
                (bricks[i].location.left + bricks[i].location.right) / 2,
                (bricks[i].location.top + bricks[i].location.bottom) / 2,
            };
            POINT ballCenter = {
                (ball->location.left + ball->location.right) / 2,
                (ball->location.top + ball->location.bottom) / 2,
            };

            bricks[i].color = RGB(rand() % 255, rand() % 255, rand() % 255);
            bricks[i].isCrash = TRUE;

            if (ballCenter.x <= brickCenter.x)
            {
                if (ballCenter.y <= brickCenter.y)
                    ball->direction = LEFT_UP;
                else if (ballCenter.y > brickCenter.y)
                    ball->direction = LEFT_DOWN;
            }
            else if (ballCenter.x > brickCenter.x)
            {
                if (ballCenter.y <= brickCenter.y)
                    ball->direction = RIGHT_UP;
                else if (ballCenter.y > brickCenter.y)
                    ball->direction = RIGHT_DOWN;
            }

            // printf("���� �߾� : (%d, %d),  �� �߾� : (%d, %d)\n", brickCenter.x, brickCenter.y, ballCenter.x, ballCenter.y);
            // printf("�ٲ� ���� %d\n", ball->direction);
        }
}

void ballCrashBar(Ball *ball, Bar *bar)
{
    RECT rcTemp;
    if (IntersectRect(&rcTemp, &ball->location, &bar->location))
    {
        int ballCenter = (ball->location.left + ball->location.right) / 2;
        int barCenter = (bar->location.left + bar->location.right) / 2;

        if (ballCenter <= barCenter)
            ball->direction = LEFT_UP;
        else if (ballCenter > barCenter)
            ball->direction = RIGHT_UP;
    }
}

// TODO
// �΋H�� ������ �ð��� ���� �۾����ٰ� ������� �Լ�
// delete();
// �Ͻ����� ������ ���� ���� ���� ������ ����� ���� ���� ȭ�鿡 ���
// bricks.isCrash �� bricks.isDelete Ȯ���ؼ� ����ϸ� �ɵ�