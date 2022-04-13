#define _CRT_SECURE_NO_WARNINGS
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include <stdio.h>
#include <stdlib.h>

#include <Windows.h>
#include <tchar.h>
#include <time.h>

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

void drawGrid(HDC, RECT, int);
void drawCircle(HDC, CIRCLE);

void listInit(LIST *);
void addCircle(LIST *, CIRCLE);

void freeList(LIST *);

void moveList(LIST *, LIST *);
// circle move set
void moveHeadCircle(CIRCLE *);
void moveTailCircle(CIRCLE *);
void moveFollow(CIRCLE *, RECT);
void intersectCheck(LIST *, LIST *);

void intersectMove(NODE *, LIST *);

void moveLeft(CIRCLE *);
void moveTop(CIRCLE *);
void moveRight(CIRCLE *);
void moveBottom(CIRCLE *);

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

    static int id;

    static int moveHeadTime;

    static const int moveTailTime = 100;
    static const int createTime = 1000;

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

        CIRCLE head;
        head.id = -1;
        head.isFollowing = FALSE;
        head.location = rcHead;
        head.prevLocation = rcHead;
        head.moveType = -1;
        head.direction = RIGHT | BOTTOM;
        head.isMoveRow = TRUE;
        head.squareMove = -1;

        addCircle(&headList, head);

        SetTimer(hWnd, TIME_CREATE, createTime, NULL);

        break;

    case WM_TIMER:
        switch (wParam)
        {
        case 1: // all circle move
            // printf("WM_TIMER case 1\n");
            moveList(&headList, &tailList);
            break;

        case 2:
            moveList(&tailList, &tailList);
            break;

        case 3: // add tail circle
            // printf("WM_TIMER case 2\n");

            printf("%d\n", id);
            if (id >= 50)
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
            moveHeadTime -= 10;
            SetTimer(hWnd, TIME_HEAD_MOVE, moveHeadTime, NULL);
            break;
        case VK_SUBTRACT:
            moveHeadTime += 10;
            SetTimer(hWnd, TIME_HEAD_MOVE, moveHeadTime, NULL);
            break;

        case 0x53: // S
            moveHeadTime = 100;
            SetTimer(hWnd, TIME_HEAD_MOVE, moveHeadTime, NULL);
            SetTimer(hWnd, TIME_TAIL_MOVE, moveTailTime, NULL);
            break;
        case 0x4A: // J
            break;
        case 0x54: // T - // TODO make function T
            KillTimer(hWnd, TIME_HEAD_MOVE);
            moveTop(&headList.link->circle);
            intersectMove(headList.link, &tailList);
            InvalidateRect(hWnd, NULL, TRUE);
            moveLeft(&headList.link->circle);
            intersectMove(headList.link, &tailList);
            InvalidateRect(hWnd, NULL, TRUE);
            headList.link->circle.direction &= (TOP | BOTTOM);
            headList.link->circle.direction |= LEFT;
            SetTimer(hWnd, TIME_HEAD_MOVE, moveHeadTime, NULL);
            break;

        case 0x51: // Q
            KillTimer(hWnd, 1);
            KillTimer(hWnd, 2);
            freeList(&headList);
            freeList(&tailList);
            PostQuitMessage(0);
            return 0;
            break;
        }
        break;

    case WM_LBUTTONDOWN:
        break;

    case WM_RBUTTONDOWN:
        break;

    case WM_PAINT:
        // printf("WM_PAINT\n");
        hdc = BeginPaint(hWnd, &ps);
        drawGrid(hdc, grid, SQUARE_COUNT);

        drawCircle(hdc, headList.link->circle);
        NODE *pos = tailList.link;
        while (pos != NULL)
        {
            drawCircle(hdc, pos->circle);
            pos = pos->next;
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
    for (int i = 1; i < size + 0.5; ++i)
    {
        MoveToEx(hdc, grid.left + square * i, grid.top, NULL);
        LineTo(hdc, grid.left + square * i, grid.bottom);
        MoveToEx(hdc, grid.left, grid.top + square * i, NULL);
        LineTo(hdc, grid.right, grid.top + square * i);
    }
    DeleteObject(hPen);
}

void drawCircle(HDC hdc, CIRCLE circle)
{
    HBRUSH hBrush, oldBrush;
    if (circle.moveType == -1)
    {
        hBrush = CreateSolidBrush(RGB(255, 255, 0));
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

void moveList(LIST *list, LIST *check)
{
    // printf("moveList\n");
    if (list->isHeadCircle)
    {
        moveHeadCircle(list);
        intersectCheck(list->link, check->link);
        if (list->link->follower != NULL)
        {
            NODE *posFollow = list->link;
            while (posFollow->follower != NULL)
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