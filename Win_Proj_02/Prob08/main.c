#define _CRT_SECURE_NO_WARNINGS

#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#include <stdio.h>

#include <Windows.h>
#include <tchar.h>
#include <time.h>

#define GRID_LOCATION 20
#define GRID_SIZE 800
#define SQUARE_SIZE GRID_SIZE / 40

typedef struct _OBJECT
{
    RECT location;
    int objectType;
    COLORREF color;
} OBJECT;

typedef struct _PLAYER
{
    RECT location;
    int shape;
    COLORREF color;
} PLAYER;

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Window Programming Lab";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

void initObject(OBJECT *, int, RECT);
void initPlayer(PLAYER *, PLAYER *, OBJECT *, int);
void crashCheck(PLAYER *, OBJECT *, int, RECT, int *);

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

    hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 1000, 880,
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

    // 그리드
    static const RECT gridLine = {
        GRID_LOCATION,
        GRID_LOCATION,
        GRID_SIZE + GRID_LOCATION,
        GRID_SIZE + GRID_LOCATION,
    };

    // 장애물
    static int objectCount;
    static OBJECT *object;

    // 플레이어
    static PLAYER player1;
    static PLAYER player2;
    static int sizeChange1, sizeChange2;
    static int turn;

    static int errorMessage;

    switch (iMessage)
    {
    case WM_CREATE:
        // 장애물 생성
        srand(time(NULL));
        objectCount = rand() % 20 + 40;
        printf("%d\n", objectCount);
        object = (OBJECT *)malloc(sizeof(OBJECT) * objectCount);

        initObject(object, objectCount, gridLine);
        initPlayer(&player1, &player2, object, objectCount);

        turn = 1;
        sizeChange1 = 0;
        sizeChange2 = 0;
        errorMessage = 0;
        break;

    case WM_KEYDOWN:
        RECT temp;
        RECT rcTemp;
        switch (wParam)
        {
        case 0x52: // r
            srand(time(NULL));
            objectCount = rand() % 20 + 40;
            printf("%d\n", objectCount);
            object = (OBJECT *)malloc(sizeof(OBJECT) * objectCount);

            initObject(object, objectCount, gridLine);
            initPlayer(&player1, &player2, object, objectCount);

            turn = 1;
            sizeChange1 = 0;
            sizeChange2 = 0;
            errorMessage = 0;
            break;

        case 0x51: // q
            PostQuitMessage(0);
            return 0;

        case 0x57: // w
            if (turn == 1)
            {
                temp = player1.location;
                OffsetRect(&player1.location, 0, -SQUARE_SIZE);
                if (player1.location.top <= GRID_LOCATION)
                    OffsetRect(&player1.location, 0, SQUARE_SIZE * 40);
                crashCheck(&player1, object, objectCount, temp, &sizeChange1);
                turn = 2;
            }
            else
                errorMessage = 1;
            break;
        case 0x41: // a
            if (turn == 1)
            {
                temp = player1.location;
                OffsetRect(&player1.location, -SQUARE_SIZE, 0);
                if (player1.location.left <= GRID_LOCATION)
                    OffsetRect(&player1.location, SQUARE_SIZE * 40, 0);
                crashCheck(&player1, object, objectCount, temp, &sizeChange1);
                turn = 2;
            }
            else
                errorMessage = 1;
            break;
        case 0x53: // s
            if (turn == 1)
            {
                temp = player1.location;
                OffsetRect(&player1.location, 0, SQUARE_SIZE);
                if (player1.location.bottom >= GRID_LOCATION + GRID_SIZE)
                    OffsetRect(&player1.location, 0, -SQUARE_SIZE * 40);
                crashCheck(&player1, object, objectCount, temp, &sizeChange1);
                turn = 2;
            }
            else
                errorMessage = 1;
            break;
        case 0x44: // d
            if (turn == 1)
            {
                temp = player1.location;
                OffsetRect(&player1.location, SQUARE_SIZE, 0);
                if (player1.location.right >= GRID_LOCATION + GRID_SIZE)
                    OffsetRect(&player1.location, -SQUARE_SIZE * 40, 0);
                crashCheck(&player1, object, objectCount, temp, &sizeChange1);
                turn = 2;
            }
            else
                errorMessage = 1;
            break;

        case VK_UP:
            if (turn == 2)
            {
                temp = player2.location;
                OffsetRect(&player2.location, 0, -SQUARE_SIZE);
                if (player2.location.top <= GRID_LOCATION)
                    OffsetRect(&player2.location, 0, SQUARE_SIZE * 40);
                crashCheck(&player2, object, objectCount, temp, &sizeChange2);
                turn = 1;
            }
            else
                errorMessage = 1;
            break;
        case VK_LEFT:
            if (turn == 2)
            {
                temp = player2.location;
                OffsetRect(&player2.location, -SQUARE_SIZE, 0);
                if (player2.location.left <= GRID_LOCATION)
                    OffsetRect(&player2.location, SQUARE_SIZE * 40, 0);
                crashCheck(&player2, object, objectCount, temp, &sizeChange2);
                turn = 1;
            }
            else
                errorMessage = 1;
            break;
        case VK_DOWN:
            if (turn == 2)
            {
                temp = player2.location;
                OffsetRect(&player2.location, 0, SQUARE_SIZE);
                if (player2.location.bottom >= GRID_LOCATION + GRID_SIZE)
                    OffsetRect(&player2.location, 0, -SQUARE_SIZE * 40);
                crashCheck(&player2, object, objectCount, temp, &sizeChange2);
                turn = 1;
            }
            else
                errorMessage = 1;
            break;
        case VK_RIGHT:
            if (turn == 2)
            {
                temp = player2.location;
                OffsetRect(&player2.location, SQUARE_SIZE, 0);
                if (player2.location.right >= GRID_LOCATION + GRID_SIZE)
                    OffsetRect(&player2.location, -SQUARE_SIZE * 40, 0);
                crashCheck(&player2, object, objectCount, temp, &sizeChange2);
                turn = 1;
            }
            else
                errorMessage = 1;
            break;
        }

        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

        // 그리드 그리기
        hPen = CreatePen(PS_SOLID, 1, BLACK_BRUSH);
        oldPen = (HPEN)SelectObject(hdc, hPen);
        FrameRect(hdc, &gridLine, hPen);
        for (int i = 1; i < 40; ++i)
        {
            MoveToEx(hdc, gridLine.left + SQUARE_SIZE * i, gridLine.top, NULL);
            LineTo(hdc, gridLine.left + SQUARE_SIZE * i, gridLine.bottom);
            MoveToEx(hdc, gridLine.left, gridLine.top + SQUARE_SIZE * i, NULL);
            LineTo(hdc, gridLine.right, gridLine.top + SQUARE_SIZE * i);
        }
        SelectObject(hdc, oldPen);
        DeleteObject(hPen);

        // 장애물 그리기
        for (int i = 0; i < objectCount; ++i)
        {
            hBrush = CreateSolidBrush(object[i].color);
            oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
            FillRect(hdc, &object[i].location, hBrush);
            SelectObject(hdc, oldBrush);
            DeleteObject(hBrush);
        }

        // 플레이어 그리기
        hBrush = CreateSolidBrush(player1.color);
        oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
        switch (player1.shape)
        {
        case 0:
            FillRect(hdc, &player1.location, hBrush);
            break;
        case 1:
            Ellipse(hdc, player1.location.left, player1.location.top, player1.location.right, player1.location.bottom);
            break;
        case 2:
            POINT triangle[3] = {
                {(player1.location.left + player1.location.right) / 2, player1.location.top},
                {player1.location.left, player1.location.bottom},
                {player1.location.right, player1.location.bottom},
            };
            Polygon(hdc, triangle, 3);
            break;
        }

        hBrush = CreateSolidBrush(player2.color);
        oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
        switch (player2.shape)
        {
        case 0:
            FillRect(hdc, &player2.location, hBrush);
            break;
        case 1:
            Ellipse(hdc, player2.location.left, player2.location.top, player2.location.right, player2.location.bottom);
            break;
        case 2:
            POINT triangle[3] = {
                {(player2.location.left + player2.location.right) / 2, player2.location.top},
                {player2.location.left, player2.location.bottom},
                {player2.location.right, player2.location.bottom},
            };
            Polygon(hdc, triangle, 3);
            break;
        }

        // 오류 메시지 출력
        if (errorMessage == 1)
        {
            TextOut(hdc, 840, 100, L"Another player's turn", lstrlen(L"Another player's turn"));
            errorMessage = 0;
        }

        SelectObject(hdc, oldBrush);
        DeleteObject(hBrush);

        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void initObject(OBJECT *object, int objectCount, RECT gridLine)
{
    srand(time(NULL));
    for (int i = 0; i < objectCount; ++i)
    {
        object[i].location.left = gridLine.left + (SQUARE_SIZE * (rand() % 39));
        object[i].location.top = gridLine.top + (SQUARE_SIZE * (rand() % 39));
        object[i].location.right = object[i].location.left + SQUARE_SIZE;
        object[i].location.bottom = object[i].location.top + SQUARE_SIZE;
        if (i > 0)
        {
            RECT rcTemp;
            for (int j = 0; j < i; ++j)
                if (IntersectRect(&rcTemp, &object[j].location, &object[i].location) != 0)
                {
                    printf("겹침\n");
                    --i;
                    continue;
                }
            // else
            //     printf("안겹침\n");
        }
        object[i].objectType = rand() % 4;
        switch (object[i].objectType)
        {
        case 0:
            object[i].color = RGB(255, 0, 0);
            break;
        case 1:
            object[i].color = RGB(rand() % 200, rand() % 255, rand() % 255);
            break;
        case 2:
            object[i].color = RGB(255, 255, 0);
            break;
        case 3:
            object[i].color = RGB(255, 125, 0);
            break;
        }
        printf("%d, %d, %d, %d, %d\n", object[i].location.left, object[i].location.top, object[i].location.right, object[i].location.bottom, object[i].objectType);
    }
}

void initPlayer(PLAYER *player1, PLAYER *player2, OBJECT *object, int objectCount)
{
    srand(time(NULL));

    player1->shape = rand() % 3;
    player2->shape = rand() % 3;

    player1->location.left = GRID_LOCATION + (SQUARE_SIZE * (rand() % 5 + 5)) + 2;
    player1->location.top = GRID_LOCATION + (SQUARE_SIZE * (rand() % 5 + 5)) + 2;
    player1->location.right = player1->location.left + SQUARE_SIZE - 4;
    player1->location.bottom = player1->location.top + SQUARE_SIZE - 4;

    player2->location.left = GRID_LOCATION + (SQUARE_SIZE * (rand() % 5 + 34)) + 2;
    player2->location.top = GRID_LOCATION + (SQUARE_SIZE * (rand() % 5 + 34)) + 2;
    player2->location.right = player2->location.left + SQUARE_SIZE - 4;
    player2->location.bottom = player2->location.top + SQUARE_SIZE - 4;

    player1->color = RGB(rand() % 255, rand() % 255, rand() % 255);
    player2->color = RGB(rand() % 255, rand() % 255, rand() % 255);

    printf("%d, %d, %d, %d\n", player1->location.left, player1->location.top, player1->location.right, player1->location.bottom);
    printf("%d, %d, %d, %d\n", player2->location.left, player2->location.top, player2->location.right, player2->location.bottom);

    for (int i = 0; i < objectCount; ++i)
    {
        RECT rcTemp;
        while (IntersectRect(&rcTemp, &player1->location, &object[i]) != 0)
        {
            player1->location.left = player1->location.left + SQUARE_SIZE;
            player1->location.top = player1->location.top + SQUARE_SIZE;
            player1->location.right = player1->location.left + SQUARE_SIZE;
            player1->location.bottom = player1->location.top + SQUARE_SIZE;
        }

        while (IntersectRect(&rcTemp, &player2->location, &object[i]) != 0)
        {
            player2->location.left = player2->location.left - SQUARE_SIZE;
            player2->location.top = player2->location.top - SQUARE_SIZE;
            player2->location.right = player2->location.left - SQUARE_SIZE;
            player2->location.bottom = player2->location.top - SQUARE_SIZE;
        }
    }
}

void crashCheck(PLAYER *player, OBJECT *object, int objectCount, RECT temp, int *sizeChange)
{
    RECT rcTemp;
    for (int i = 0; i < objectCount; ++i)
    {
        if (IntersectRect(&rcTemp, &player->location, &object[i].location) != 0)
        {
            switch (object[i].objectType)
            {
            case 0:
                player->location = temp;
                break;
            case 1:
                player->color = object[i].color;
                break;
            case 2:
                if (*sizeChange)
                {
                    InflateRect(&player->location, 2, 2);
                    *sizeChange = 0;
                }
                else
                {
                    InflateRect(&player->location, -2, -2);
                    *sizeChange = 1;
                }
                break;
            case 3:
                ++player->shape;
                if (player->shape == 3)
                    player->shape = 0;
                break;
            }
        }
    }
}