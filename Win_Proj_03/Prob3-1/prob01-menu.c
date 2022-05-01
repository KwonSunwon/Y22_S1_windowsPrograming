#define _CRT_SECURE_NO_WARNINGS
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include <stdio.h>
#include <stdlib.h>

#include <Windows.h>
#include <tchar.h>
#include <time.h>

#include "resource.h"

#define GRID_LOCATION 20
#define GRID_SIZE 800
#define SQUARE_COUNT 40
#define SQUARE_SIZE (GRID_SIZE / SQUARE_COUNT)

#define TIME_HEAD_MOVE 1
#define TIME_TAIL_MOVE 2
#define TIME_CREATE 3

#define LEFT 0b1000
#define TOP 0b0100
#define RIGHT 0b0010
#define BOTTOM 0b0001

typedef struct _CIRCLE
{
    int id;

    BOOL isFollowing;
    RECT location;
    RECT prevLocation;

    int moveType; // if -1 , head circle

    int direction;
    BOOL isMoveRow;
    int squareMove;
} CIRCLE;

typedef struct _NODE
{
    CIRCLE circle;
    struct _NODE *next;
    struct _NODE *follower;
} NODE;

typedef struct _LIST
{
    BOOL isHeadCircle;
    NODE *link;
} LIST;

typedef struct _OBJECT
{
    RECT location[50];
    int count;
} OBJECT;

void drawGrid(HDC, RECT, int);
void drawCircle(HDC, CIRCLE, COLORREF);
void drawRectangle(HDC, CIRCLE, COLORREF);

void listInit(LIST *, BOOL);
void addCircle(LIST *, CIRCLE);

void freeList(LIST *);

void moveList(LIST *, LIST *, OBJECT *, LIST *);

// circle move set
void moveHeadCircle(LIST *);
void moveTailCircle(CIRCLE *);
void moveFollow(CIRCLE *, RECT);
void intersectCheck(NODE *, NODE *);

void intersectMove(NODE *, LIST *);

void moveLeft(CIRCLE *);
void moveTop(CIRCLE *);
void moveRight(CIRCLE *);
void moveBottom(CIRCLE *);

void drawObject(HDC, RECT);
void intersectObject(CIRCLE *, OBJECT *, BOOL);

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

    hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 900, 900, NULL, (HMENU)NULL, hInstance, NULL);
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

    static const RECT grid = {
        GRID_LOCATION,
        GRID_LOCATION,
        GRID_SIZE + GRID_LOCATION,
        GRID_SIZE + GRID_LOCATION,
    };

    static LIST headList;
    static LIST tailList;

    static BOOL headIsCircle;
    static COLORREF headColor;

    static int id;
    static int tailCircleCount;

    static int moveHeadTime;

    static int moveTailTime;
    static int createTime;

    static int jumpState;
    static int speedUpState;
    static BOOL speedUpFlag;

    static OBJECT object;

    int mx, my;
    int cx, cy;
    int x, y;
    NODE *pos;
    CIRCLE head;

    switch (iMessage)
    {
    case WM_CREATE:
        srand(time(NULL));
        listInit(&headList, TRUE);
        listInit(&tailList, FALSE);

        id = 0;

        RECT rcHead = {
            GRID_LOCATION,
            GRID_LOCATION,
            GRID_LOCATION + (GRID_SIZE / SQUARE_COUNT),
            GRID_LOCATION + (GRID_SIZE / SQUARE_COUNT)};
        head.id = -1;
        head.isFollowing = FALSE;
        head.location = rcHead;
        head.prevLocation = rcHead;
        head.moveType = -1;
        head.direction = RIGHT | BOTTOM;
        head.isMoveRow = TRUE;
        head.squareMove = -1;

        addCircle(&headList, head);

        moveHeadTime = 100;
        moveTailTime = 100;
        createTime = 1000;
        SetTimer(hWnd, TIME_CREATE, createTime, NULL);

        jumpState = 0;
        speedUpFlag = FALSE;
        speedUpState = -1;
        tailCircleCount = 20;

        headColor = RGB(255, 255, 0);
        headIsCircle = TRUE;

        object.count = 0;
        break;

    case WM_TIMER:
        switch (wParam)
        {
        case TIME_HEAD_MOVE: // all circle move
            if (!jumpState)
                moveList(&headList, &tailList, &object, &headList);
            else
            {
                headList.link->circle.prevLocation = headList.link->circle.location;
                if (jumpState == 3)
                {
                    if (headList.link->circle.isMoveRow)
                    {
                        moveTop(&headList.link->circle);
                    }
                    else
                    {
                        moveRight(&headList.link->circle);
                    }
                }
                else if (jumpState == 2)
                {
                    if (headList.link->circle.isMoveRow)
                    {
                        if ((headList.link->circle.direction & LEFT) == LEFT)
                        {
                            moveLeft(&headList.link->circle);
                        }
                        else
                        {
                            moveRight(&headList.link->circle);
                        }
                    }
                    else
                    {
                        if ((headList.link->circle.direction & TOP) == TOP)
                        {
                            moveTop(&headList.link->circle);
                        }
                        else
                        {
                            moveBottom(&headList.link->circle);
                        }
                    }
                }
                else if (jumpState == 1)
                {
                    if (headList.link->circle.isMoveRow)
                    {
                        moveBottom(&headList.link->circle);
                    }
                    else
                    {
                        moveLeft(&headList.link->circle);
                    }
                }
                intersectMove(headList.link, &tailList);
                --jumpState;
            }

            if (speedUpFlag)
            {
                if (speedUpState == 0)
                {
                    KillTimer(hWnd, TIME_HEAD_MOVE);
                    moveHeadTime += 50;
                    SetTimer(hWnd, TIME_HEAD_MOVE, moveHeadTime, NULL);
                    speedUpFlag = FALSE;
                }
                --speedUpState;
            }

            break;

        case TIME_TAIL_MOVE:
            moveList(&tailList, &tailList, &object, &headList);
            break;

        case TIME_CREATE: // add tail circle
            // printf("%d\n", id);
            if (id >= tailCircleCount)
                break;

            RECT tailRect;
            tailRect.left = GRID_LOCATION + (SQUARE_SIZE * (rand() % SQUARE_COUNT));
            tailRect.top = GRID_LOCATION + (SQUARE_SIZE * (rand() % SQUARE_COUNT));
            tailRect.right = tailRect.left + SQUARE_SIZE;
            tailRect.bottom = tailRect.top + SQUARE_SIZE;

            int direction = 0b0;
            switch (rand() % 4)
            {
            case 0:
                direction = LEFT;
                break;
            case 1:
                direction = TOP;
                break;
            case 2:
                direction = RIGHT;
                break;
            case 3:
                direction = BOTTOM;
                break;
            }

            int moveType = rand() % 3;

            CIRCLE tail;
            tail.id = id++;
            tail.isFollowing = FALSE;
            tail.location = tailRect;
            tail.prevLocation = tailRect;
            tail.moveType = moveType;
            tail.direction = direction;
            tail.isMoveRow = TRUE;
            tail.squareMove = 5;

            addCircle(&tailList, tail);
            break;
        }
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_LEFT:
            headList.link->circle.isMoveRow = TRUE;
            headList.link->circle.direction &= (TOP | BOTTOM);
            headList.link->circle.direction |= LEFT;
            break;
        case VK_RIGHT:
            headList.link->circle.isMoveRow = TRUE;
            headList.link->circle.direction &= (TOP | BOTTOM);
            headList.link->circle.direction |= RIGHT;
            break;
        case VK_UP:
            headList.link->circle.isMoveRow = FALSE;
            headList.link->circle.direction &= (LEFT | RIGHT);
            headList.link->circle.direction |= TOP;
            break;
        case VK_DOWN:
            headList.link->circle.isMoveRow = FALSE;
            headList.link->circle.direction &= (LEFT | RIGHT);
            headList.link->circle.direction |= BOTTOM;
            break;

        case VK_ADD:
            KillTimer(hWnd, TIME_HEAD_MOVE, moveHeadTime, NULL);
            moveHeadTime -= 10;
            SetTimer(hWnd, TIME_HEAD_MOVE, moveHeadTime, NULL);
            break;
        case VK_SUBTRACT:
            KillTimer(hWnd, TIME_HEAD_MOVE, moveHeadTime, NULL);
            moveHeadTime += 10;
            SetTimer(hWnd, TIME_HEAD_MOVE, moveHeadTime, NULL);
            break;

        case 0x53: // S
            moveHeadTime = 100;
            moveTailTime = 100;
            SetTimer(hWnd, TIME_HEAD_MOVE, moveHeadTime, NULL);
            SetTimer(hWnd, TIME_TAIL_MOVE, moveTailTime, NULL);
            break;

        case 0x4A: // J
            jumpState = 3;
            break;

        case 0x54: // T
            KillTimer(hWnd, TIME_HEAD_MOVE);
            if (headList.link->circle.isMoveRow)
            {
                if ((headList.link->circle.direction & LEFT) == LEFT)
                {
                    moveBottom(&headList.link->circle);
                    intersectMove(headList.link, &tailList);
                    InvalidateRect(hWnd, NULL, TRUE);
                    moveRight(&headList.link->circle);
                    headList.link->circle.direction &= (TOP | BOTTOM);
                    headList.link->circle.direction |= RIGHT;
                }
                else
                {
                    moveTop(&headList.link->circle);
                    intersectMove(headList.link, &tailList);
                    InvalidateRect(hWnd, NULL, TRUE);
                    moveLeft(&headList.link->circle);
                    headList.link->circle.direction &= (TOP | BOTTOM);
                    headList.link->circle.direction |= LEFT;
                }
            }
            else
            {
                if ((headList.link->circle.direction & TOP) == TOP)
                {
                    moveLeft(&headList.link->circle);
                    intersectMove(headList.link, &tailList);
                    InvalidateRect(hWnd, NULL, TRUE);
                    moveBottom(&headList.link->circle);
                    headList.link->circle.direction &= (LEFT | RIGHT);
                    headList.link->circle.direction |= BOTTOM;
                }
                else
                {
                    moveRight(&headList.link->circle);
                    intersectMove(headList.link, &tailList);
                    InvalidateRect(hWnd, NULL, TRUE);
                    moveTop(&headList.link->circle);
                    headList.link->circle.direction &= (LEFT | RIGHT);
                    headList.link->circle.direction |= TOP;
                }
            }
            intersectMove(headList.link, &tailList);
            InvalidateRect(hWnd, NULL, TRUE);
            SetTimer(hWnd, TIME_HEAD_MOVE, moveHeadTime, NULL);
            break;

        case 0x51: // Q
            KillTimer(hWnd, TIME_HEAD_MOVE);
            KillTimer(hWnd, TIME_TAIL_MOVE);
            KillTimer(hWnd, TIME_CREATE);
            freeList(&headList);
            freeList(&tailList);
            PostQuitMessage(0);
            return 0;
        }
        break;

        // TODO make lclick
        // NOTE lclick
        // 머리원 선택 - 속도업 카운트 만들어서 5주고, 속도 올린다음 timer에서 하나씩 줄여서 0 되면 원래 속도로 복귀
        // 빈공간 선택 - 좌표는 알았으니까 둘이 비교해서 짧은 쪽으로 방향 주고 먼 쪽은 벽 부딪일 때 회전 방향으로
        // 꼬리원 선택 - 일단 선택된 원부터 전부 isFollowing false로 해주고
        //              false 해줌과 동시에 following 주소를 NULL로 만들어줘야함
    case WM_LBUTTONDOWN:
        mx = LOWORD(lParam);
        my = HIWORD(lParam);

        pos = headList.link;

        POINT mouse = {mx, my};

        BOOL flag = FALSE;

        if (PtInRect(&headList.link->circle.location, mouse)) // 주인공 원
        {
            // printf("머리원 선택\n");
            speedUpState = 5;
            speedUpFlag = TRUE;
            KillTimer(hWnd, TIME_HEAD_MOVE);
            moveHeadTime -= 50;
            SetTimer(hWnd, TIME_HEAD_MOVE, moveHeadTime, NULL);
            flag = TRUE;
        }
        else if (pos->follower != NULL)
        {
            pos = pos->follower;
            if (PtInRect(&pos->circle.location, mouse))
            {
                headList.link->follower = NULL;
                if (pos->follower == NULL) // 꼬리원이 하나 밖에 없는 경우
                {
                    pos->circle.isFollowing = FALSE;
                }
                else
                {
                    while (pos->follower != NULL)
                    {
                        NODE *delFollow = pos->follower;
                        pos->follower = delFollow->follower;
                        delFollow->circle.isFollowing = FALSE;
                        delFollow->follower = NULL;
                    }
                    pos->circle.isFollowing = FALSE;
                }
            }
            else
                while (pos->follower != NULL)
                {
                    if (PtInRect(&pos->follower->circle.location, mouse))
                    {
                        NODE *delFollow = pos->follower;
                        pos->follower = NULL;
                        pos = delFollow;
                        while (pos->follower != NULL)
                        {
                            delFollow = pos->follower;
                            pos->follower = delFollow->follower;
                            delFollow->circle.isFollowing = FALSE;
                            delFollow->follower = NULL;
                        }
                        pos->circle.isFollowing = FALSE;
                        break;
                    }
                    pos = pos->follower;
                }
            flag = TRUE;
        }
        if (flag) // 빈공간
        {
            printf("빈공간 선택\n");
            cx = headList.link->circle.location.left + 10;
            cy = headList.link->circle.location.top + 10;

            x = cx - mx;
            y = cy - my;

            // printf("x : %d, y : %d\n", x, y);

            if (abs(x) < abs(y)) // x 축이 더 가까움 -> x 쪽으로 이동
            {
                headList.link->circle.isMoveRow = TRUE;
                if (x > 0) // x 가 양수 -> 왼쪽으로 이동
                {
                    headList.link->circle.direction &= (TOP | BOTTOM);
                    headList.link->circle.direction |= LEFT;
                }
                else
                {
                    headList.link->circle.direction &= (TOP | BOTTOM);
                    headList.link->circle.direction |= RIGHT;
                }
            }
            else // y 쪽으로 이동
            {
                headList.link->circle.isMoveRow = FALSE;
                if (y > 0)
                {
                    headList.link->circle.direction &= (LEFT | RIGHT);
                    headList.link->circle.direction |= TOP;
                }
                else
                {
                    headList.link->circle.direction &= (LEFT | RIGHT);
                    headList.link->circle.direction |= BOTTOM;
                }
            }
        }
        break;

    case WM_RBUTTONDOWN:
        mx = (LOWORD(lParam) / 20) * 20;
        my = (HIWORD(lParam) / 20) * 20;

        RECT rcTemp = {mx, my, mx + 20, my + 20};
        object.location[object.count++] = rcTemp;

        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_COMMAND:
        switch (wParam)
        {
        case ID_GAME_START:
            SetTimer(hWnd, TIME_HEAD_MOVE, moveHeadTime, NULL);
            SetTimer(hWnd, TIME_TAIL_MOVE, moveTailTime, NULL);
            break;

        case ID_GAME_RESET:
            KillTimer(hWnd, TIME_HEAD_MOVE);
            KillTimer(hWnd, TIME_TAIL_MOVE);
            KillTimer(hWnd, TIME_CREATE);
            freeList(&headList);
            freeList(&tailList);

            listInit(&headList, TRUE);
            listInit(&tailList, FALSE);

            id = 0;

            RECT rcHead = {
                GRID_LOCATION,
                GRID_LOCATION,
                GRID_LOCATION + (GRID_SIZE / SQUARE_COUNT),
                GRID_LOCATION + (GRID_SIZE / SQUARE_COUNT)};

            head.id = -1;
            head.isFollowing = FALSE;
            head.location = rcHead;
            head.prevLocation = rcHead;
            head.moveType = -1;
            head.direction = RIGHT | BOTTOM;
            head.isMoveRow = TRUE;
            head.squareMove = -1;

            addCircle(&headList, head);

            moveHeadTime = 100;
            moveTailTime = 100;
            createTime = 1000;
            SetTimer(hWnd, TIME_CREATE, createTime, NULL);

            jumpState = 0;
            speedUpFlag = FALSE;
            speedUpState = -1;
            tailCircleCount = 20;

            headColor = RGB(255, 255, 0);
            headIsCircle = TRUE;
            object.count = 0;
            break;

        case ID_GAME_END:
            KillTimer(hWnd, TIME_HEAD_MOVE);
            KillTimer(hWnd, TIME_TAIL_MOVE);
            freeList(&headList);
            freeList(&tailList);
            PostQuitMessage(0);
            return 0;
            break;

        case ID_SPEED_FAST:
            KillTimer(hWnd, TIME_HEAD_MOVE, moveHeadTime, NULL);
            KillTimer(hWnd, TIME_TAIL_MOVE, moveTailTime, NULL);
            KillTimer(hWnd, TIME_CREATE, createTime, NULL);
            moveHeadTime = 50;
            moveTailTime = 50;
            createTime = 800;
            SetTimer(hWnd, TIME_HEAD_MOVE, moveHeadTime, NULL);
            SetTimer(hWnd, TIME_TAIL_MOVE, moveTailTime, NULL);
            SetTimer(hWnd, TIME_CREATE, createTime, NULL);
            break;

        case ID_SPEED_MEDIUM:
            KillTimer(hWnd, TIME_HEAD_MOVE, moveHeadTime, NULL);
            KillTimer(hWnd, TIME_TAIL_MOVE, moveTailTime, NULL);
            KillTimer(hWnd, TIME_CREATE, createTime, NULL);
            moveHeadTime = 100;
            moveTailTime = 100;
            createTime = 1000;
            SetTimer(hWnd, TIME_HEAD_MOVE, moveHeadTime, NULL);
            SetTimer(hWnd, TIME_TAIL_MOVE, moveTailTime, NULL);
            SetTimer(hWnd, TIME_CREATE, createTime, NULL);
            break;

        case ID_SPEED_SLOW:
            KillTimer(hWnd, TIME_HEAD_MOVE, moveHeadTime, NULL);
            KillTimer(hWnd, TIME_TAIL_MOVE, moveTailTime, NULL);
            KillTimer(hWnd, TIME_CREATE, createTime, NULL);
            moveHeadTime = 200;
            moveTailTime = 200;
            createTime = 1200;
            SetTimer(hWnd, TIME_HEAD_MOVE, moveHeadTime, NULL);
            SetTimer(hWnd, TIME_TAIL_MOVE, moveTailTime, NULL);
            SetTimer(hWnd, TIME_CREATE, createTime, NULL);
            break;

        case ID_COLOR_CYAN:
            headColor = RGB(0, 255, 255);
            break;
        case ID_COLOR_MAGENTA:
            headColor = RGB(255, 0, 255);
            break;
        case ID_COLOR_YELLOW:
            headColor = RGB(255, 255, 0);
            break;

        case ID_SHAPE_CIRCLE:
            headIsCircle = TRUE;
            break;
        case ID_SHAPE_RECTANGLE:
            headIsCircle = FALSE;
            break;

        case ID_COUNT_20:
            tailCircleCount = 20;
            id = 0;
            freeList(&tailList);
            listInit(&tailList, FALSE);
            head.id = -1;
            head.isFollowing = FALSE;
            head.location = headList.link->circle.location;
            head.prevLocation = headList.link->circle.prevLocation;
            head.moveType = -1;
            head.direction = headList.link->circle.direction;
            head.isMoveRow = headList.link->circle.isMoveRow;
            head.squareMove = -1;
            freeList(&headList);
            listInit(&headList, TRUE);
            addCircle(&headList, head);
            break;

        case ID_COUNT_25:
            tailCircleCount = 25;
            id = 0;
            freeList(&tailList);
            listInit(&tailList, FALSE);
            head.id = -1;
            head.isFollowing = FALSE;
            head.location = headList.link->circle.location;
            head.prevLocation = headList.link->circle.prevLocation;
            head.moveType = -1;
            head.direction = headList.link->circle.direction;
            head.isMoveRow = headList.link->circle.isMoveRow;
            head.squareMove = -1;
            freeList(&headList);
            listInit(&headList, TRUE);
            addCircle(&headList, head);
            break;

        case ID_COUNT_30:
            tailCircleCount = 30;
            id = 0;
            freeList(&tailList);
            listInit(&tailList, FALSE);
            head.id = -1;
            head.isFollowing = FALSE;
            head.location = headList.link->circle.location;
            head.prevLocation = headList.link->circle.prevLocation;
            head.moveType = -1;
            head.direction = headList.link->circle.direction;
            head.isMoveRow = headList.link->circle.isMoveRow;
            head.squareMove = -1;
            freeList(&headList);
            listInit(&headList, TRUE);
            addCircle(&headList, head);
            break;
        }
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_PAINT:
        // printf("WM_PAINT\n");
        hdc = BeginPaint(hWnd, &ps);
        drawGrid(hdc, grid, SQUARE_COUNT);

        if (headIsCircle)
            drawCircle(hdc, headList.link->circle, headColor);
        else
            drawRectangle(hdc, headList.link->circle, headColor);
        pos = tailList.link;
        while (pos != NULL)
        {
            drawCircle(hdc, pos->circle, headColor);
            pos = pos->next;
        }
        for (int i = 0; i < object.count; ++i)
        {
            drawObject(hdc, object.location[i]);
        }

        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        KillTimer(hWnd, TIME_HEAD_MOVE);
        KillTimer(hWnd, TIME_TAIL_MOVE);
        KillTimer(hWnd, TIME_CREATE);
        freeList(&headList);
        freeList(&tailList);
        PostQuitMessage(0);
        return 0;
    }
    return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void drawGrid(HDC hdc, RECT grid, int size)
{
    HPEN hPen = CreatePen(PS_SOLID, 1, BLACK_BRUSH);
    float square = (grid.right - grid.left) / size;
    FrameRect(hdc, &grid, hPen);
    for (int i = 1; i < size; ++i)
    {
        MoveToEx(hdc, grid.left + square * i, grid.top, NULL);
        LineTo(hdc, grid.left + square * i, grid.bottom);
        MoveToEx(hdc, grid.left, grid.top + square * i, NULL);
        LineTo(hdc, grid.right, grid.top + square * i);
    }
    DeleteObject(hPen);
}

void drawCircle(HDC hdc, CIRCLE circle, COLORREF headColor)
{
    HBRUSH hBrush, oldBrush;
    if (circle.moveType == -1)
    {
        hBrush = CreateSolidBrush(headColor);
    }
    else
    {
        hBrush = CreateSolidBrush(RGB(255, 100, 0));
    }
    oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    Ellipse(hdc, circle.location.left, circle.location.top, circle.location.right, circle.location.bottom);
    SelectObject(hdc, oldBrush);
    DeleteObject(hBrush);
}

void drawRectangle(HDC hdc, CIRCLE circle, COLORREF headColor)
{
    HBRUSH hBrush, oldBrush;
    if (circle.moveType == -1)
    {
        hBrush = CreateSolidBrush(headColor);
    }
    else
    {
        hBrush = CreateSolidBrush(RGB(255, 100, 0));
    }
    oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    Rectangle(hdc, circle.location.left, circle.location.top, circle.location.right, circle.location.bottom);
    SelectObject(hdc, oldBrush);
    DeleteObject(hBrush);
}

void listInit(LIST *list, BOOL isHead)
{
    list->isHeadCircle = isHead;
    list->link = NULL;
}

void addCircle(LIST *list, CIRCLE circle)
{
    NODE *newNode = (NODE *)malloc(sizeof(NODE));
    newNode->circle = circle;
    newNode->next = NULL;
    newNode->follower = NULL;

    if (list->link == NULL)
    {
        list->link = newNode;
    }
    else
    {
        NODE *pos = list->link;
        while (pos->next != NULL)
        {
            pos = pos->next;
        }
        pos->next = newNode;
    }
}

void moveList(LIST *list, LIST *check, OBJECT *object, LIST *head)
{
    // printf("moveList\n");
    if (list->isHeadCircle)
    {
        moveHeadCircle(list);
        intersectObject(&list->link->circle, object, list->isHeadCircle);
        intersectCheck(list->link, check->link);
        if (list->link->follower != NULL)
        {
            NODE *posFollow = list->link;
            while (posFollow->follower != NULL && posFollow->follower->circle.isFollowing)
            {
                moveFollow(posFollow->follower, posFollow->circle.prevLocation);
                posFollow = posFollow->follower;
            }
        }
    }
    else
    {
        // printf("moveList else\n");
        if (list->link == NULL)
            return;
        NODE *pos = list->link;
        while (pos->next != NULL)
        {
            if (!pos->circle.isFollowing)
            {
                moveTailCircle(&pos->circle);
                intersectObject(&pos->circle, object, list->isHeadCircle);
                intersectCheck(pos, check->link);
                intersectCheck(head->link, check->link);
                if (pos->follower != NULL && pos->follower->circle.isFollowing)
                {
                    NODE *posFollow = pos;
                    while (posFollow->follower != NULL)
                    {
                        moveFollow(posFollow->follower, posFollow->circle.prevLocation);
                        posFollow = posFollow->follower;
                    }
                }
            }
            pos = pos->next;
        }
    }
}

void moveFollow(CIRCLE *circle, RECT location)
{
    // printf("moveFollow\n");
    circle->prevLocation = circle->location;
    circle->location = location;
}

void moveHeadCircle(LIST *list)
{
    CIRCLE *circle = &list->link->circle;

    circle->prevLocation = circle->location;

    if (circle->isMoveRow) // more row
    {
        if ((circle->direction & LEFT) == LEFT)
            if (circle->location.left <= GRID_LOCATION)
            {
                if (circle->direction & TOP)
                    moveTop(circle);
                else if (circle->direction & BOTTOM)
                    moveBottom(circle);
                circle->direction ^= (LEFT | RIGHT);
            }
            else
                moveLeft(circle);
        else if ((circle->direction & RIGHT) == RIGHT)
        {
            if (circle->location.right >= GRID_LOCATION + GRID_SIZE)
            {
                if (circle->direction & TOP)
                    moveTop(circle);
                else if (circle->direction & BOTTOM)
                    moveBottom(circle);
                circle->direction ^= (LEFT | RIGHT);
            }
            else
                moveRight(circle);
        }
    }
    else // move col
    {
        if ((circle->direction & TOP) == TOP)
        {
            if (circle->location.top <= GRID_LOCATION)
            {
                if (circle->direction & LEFT)
                    moveLeft(circle);
                else if (circle->direction & RIGHT)
                    moveRight(circle);
                circle->direction ^= (TOP | BOTTOM);
            }
            else
                moveTop(circle);
        }
        else if ((circle->direction & BOTTOM) == BOTTOM)
        {
            if (circle->location.bottom >= GRID_LOCATION + GRID_SIZE)
            {
                if (circle->direction & LEFT)
                    moveLeft(circle);
                else if (circle->direction & RIGHT)
                    moveRight(circle);
                circle->direction ^= (TOP | BOTTOM);
            }
            else
                moveBottom(circle);
        }
    }
    // printf("%d,%d,%d,%d\n", circle->location.left, circle->location.top, circle->location.right, circle->location.bottom);
}

void moveTailCircle(CIRCLE *circle)
{
    // printf("moveTailCircle\n");
    circle->prevLocation = circle->location;

    switch (circle->moveType)
    {
    case 0:
        switch (circle->direction)
        {
        case LEFT:
            moveLeft(circle);
            break;
        case TOP:
            moveTop(circle);
            break;
        case RIGHT:
            moveRight(circle);
            break;
        case BOTTOM:
            moveBottom(circle);
            break;
        }
        break;
    case 1:
        if (circle->squareMove == 0)
        {
            switch (circle->direction)
            {
            case LEFT:
                circle->direction = TOP;
                moveTop(circle);
                break;
            case TOP:
                circle->direction = RIGHT;
                moveRight(circle);
                break;
            case RIGHT:
                circle->direction = BOTTOM;
                moveBottom(circle);
                break;
            case BOTTOM:
                circle->direction = LEFT;
                moveLeft(circle);
                break;
            }
            circle->squareMove = 5;
        }
        else
        {
            switch (circle->direction)
            {
            case LEFT:
                moveLeft(circle);
                break;
            case TOP:
                moveTop(circle);
                break;
            case RIGHT:
                moveRight(circle);
                break;
            case BOTTOM:
                moveBottom(circle);
                break;
            }
            --circle->squareMove;
        }
        break;
    }
}

void intersectCheck(NODE *head, NODE *tail)
{
    // printf("intersectCheck\n");
    RECT rcTemp;
    while (tail != NULL)
    {
        if (head->circle.id != tail->circle.id)
            if (!tail->circle.isFollowing)
                if (IntersectRect(&rcTemp, &head->circle.location, &tail->circle.location))
                {
                    NODE *posHead = head;
                    while (posHead->follower != NULL)
                    {
                        posHead = posHead->follower;
                    }
                    posHead->follower = tail;
                    tail->circle.isFollowing = TRUE;
                }
        tail = tail->next;
    }
}

void intersectMove(NODE *pos, LIST *check)
{
    intersectCheck(pos, check->link);
    if (pos->follower != NULL)
    {
        NODE *posFollow = pos;
        while (posFollow->follower != NULL)
        {
            moveFollow(posFollow->follower, posFollow->circle.prevLocation);
            posFollow = posFollow->follower;
        }
    }
}

// circle move set
void moveLeft(CIRCLE *circle)
{
    if (circle->location.left <= GRID_LOCATION)
        circle->direction ^= (LEFT | RIGHT);
    else
        OffsetRect(&circle->location, -SQUARE_SIZE, 0);
}
void moveTop(CIRCLE *circle)
{
    if (circle->location.top <= GRID_LOCATION)
        circle->direction ^= (TOP | BOTTOM);
    else
        OffsetRect(&circle->location, 0, -SQUARE_SIZE);
}
void moveRight(CIRCLE *circle)
{
    if (circle->location.right >= GRID_LOCATION + GRID_SIZE)
        circle->direction ^= (LEFT | RIGHT);
    else
        OffsetRect(&circle->location, SQUARE_SIZE, 0);
}
void moveBottom(CIRCLE *circle)
{
    if (circle->location.bottom >= GRID_LOCATION + GRID_SIZE)
        circle->direction ^= (TOP | BOTTOM);
    else
        OffsetRect(&circle->location, 0, SQUARE_SIZE);
}

void freeList(LIST *list)
{
    while (list->link != NULL)
    {
        NODE *delNode = list->link;
        list->link = delNode->next;
        free(delNode);
    }
}

void drawObject(HDC hdc, RECT object)
{
    HBRUSH hBrush = CreateSolidBrush(RGB(200, 20, 20));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    Rectangle(hdc, object.left, object.top, object.right, object.bottom);
    SelectObject(hdc, oldBrush);
    DeleteObject(hBrush);
}

void intersectObject(CIRCLE *circle, OBJECT *object, BOOL isHead)
{
    RECT rcTemp;
    for (int i = 0; i < object->count; ++i)
        if (IntersectRect(&rcTemp, &circle->location, &object->location[i]))
        {
            circle->location = circle->prevLocation;
            if (isHead)
            {
                if (circle->isMoveRow)
                    circle->isMoveRow = FALSE;
                else
                    circle->isMoveRow = TRUE;
            }
            else
            {
                switch (circle->direction)
                {
                case LEFT:
                    circle->direction = BOTTOM;
                    break;
                case TOP:
                    circle->direction = LEFT;
                    break;
                case RIGHT:
                    circle->direction = TOP;
                    break;
                case BOTTOM:
                    circle->direction = RIGHT;
                    break;
                }
            }
        }
}