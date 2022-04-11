#define _CRT_SECURE_NO_WARNINGS
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include <stdio.h>

#include <Windows.h>
#include <tchar.h>

#define GRID_LOCATION 20
#define GRID_SIZE 800
#define SQUARE_COUNT 40
#define SQUARE_SIZE (GRID_SIZE / SQUARE_COUNT)

#define LEFT 0b1000
#define TOP 0b0100
#define RIGHT 0b0010
#define BOTTOM 0b0001

typedef struct _Circle
{
    BOOL followFlag;     // false 독립적으로 이동, true 앞의 원을 따라감
    int followCircleIdx; // 따라가는 원의 index

    int nextCircleIdx; // 따라오는 원의 index

    RECT location;
    RECT prevLocation;
    int moveType; // 0 이면 headCircle

    int direction;
    BOOL rowOrCol;
} Circle;

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Window Programming Lab";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

void drawGrid(HDC, RECT, int);
void drawCircle(HDC, Circle);
void moveTailCircle(Circle *, int); // 따라가는 원이 불려서 앞의 원의 prevLocation 을 받아옴, moveType에 따라 이동함
void intersectCircle(Circle *, Circle *);

void moveHeadCircle(Circle *);

void moveLeft(Circle *);
void moveTop(Circle *);
void moveRight(Circle *);
void moveBottom(Circle *);

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

    HBRUSH hBrush, oldBrush;

    static const RECT grid = {
        GRID_LOCATION,
        GRID_LOCATION,
        GRID_SIZE + GRID_LOCATION,
        GRID_SIZE + GRID_LOCATION,
    };

    static Circle headCircle;

    static Circle arrTailCircle[SQUARE_COUNT * SQUARE_COUNT];
    static int countTailCircle;

    switch (iMessage)
    {
    case WM_CREATE:
        RECT temp = {100, 100, 120, 120};
        arrTailCircle[0].prevLocation = temp;
        arrTailCircle[0].nextCircleIdx = 50;

        arrTailCircle[40].followFlag = TRUE;
        arrTailCircle[40].followCircleIdx = 0;
        RECT temp2 = {200, 200, 220, 220};
        arrTailCircle[40].location = temp2;

        RECT rcTemp = {
            GRID_LOCATION,
            GRID_LOCATION,
            GRID_LOCATION + (GRID_SIZE / SQUARE_COUNT),
            GRID_LOCATION + (GRID_SIZE / SQUARE_COUNT)};
        RECT rcTemp2 = {0, 0, 0, 0};
        headCircle.followFlag = FALSE;
        headCircle.followCircleIdx = -1;
        headCircle.moveType = -1;
        headCircle.nextCircleIdx = -1;
        headCircle.rowOrCol = FALSE;
        headCircle.direction = RIGHT | BOTTOM;
        headCircle.location = rcTemp;
        headCircle.prevLocation = rcTemp2;

        SetTimer(hWnd, 1, 100, NULL);

        printf("%d, %d\n", headCircle.direction, RIGHT | BOTTOM);

        break;

    case WM_TIMER:
        moveHeadCircle(&headCircle);
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_KEYDOWN:
        break;

    case WM_LBUTTONDOWN:
        break;

    case WM_RBUTTONDOWN:
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        drawGrid(hdc, grid, 40);

        drawCircle(hdc, headCircle);
        // printf("%d\n", arrTailCircle[40].location.left);
        // arrTailCircle[40].location = arrTailCircle[arrTailCircle->followCircleIdx].prevLocation;
        // printf("%d\n", arrTailCircle[40].location.left);

        moveTailCircle(arrTailCircle, 40);
        // printf("%d\n", arrTailCircle[40].location.left);

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
    FrameRect(hdc, &grid, hPen);
    for (int i = 1; i < size + 0.5; ++i)
    {
        MoveToEx(hdc, grid.left + square * i, grid.top, NULL);
        LineTo(hdc, grid.left + square * i, grid.bottom);
        MoveToEx(hdc, grid.left, grid.top + square * i, NULL);
        LineTo(hdc, grid.right, grid.top + square * i);
    }
}

void drawCircle(HDC hdc, Circle circle)
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
}

void moveTailCircle(Circle *circle, int idx)
{
    if (circle[idx].followFlag)
    {
        // printf("%d\n", circle[idx].location.left);
        circle[idx].location = circle[circle[idx].followCircleIdx].prevLocation;
        // printf("%d\n", circle[idx].location.left);
    }
    else
    {
        switch (circle->moveType)
        {
        case 1:
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
        case 2:
            break;
        case 3: // 제자리 그대로
            break;
        }
    }
}

void moveHeadCircle(Circle *circle)
{
    circle->prevLocation = circle->location;

    if (circle->rowOrCol)
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
    else
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

    printf("%d,%d,%d,%d\n", circle->location.left, circle->location.top, circle->location.right, circle->location.bottom);
}

void moveLeft(Circle *circle)
{
    if (circle->location.left <= GRID_LOCATION)
        circle->direction ^= (LEFT | RIGHT);
    else
        OffsetRect(&circle->location, -SQUARE_SIZE, 0);
}
void moveTop(Circle *circle)
{
    if (circle->location.top <= GRID_LOCATION)
        circle->direction ^= (TOP | BOTTOM);
    else
        OffsetRect(&circle->location, 0, -SQUARE_SIZE);
}
void moveRight(Circle *circle)
{
    if (circle->location.right >= GRID_LOCATION + GRID_SIZE)
        circle->direction ^= (LEFT | RIGHT);
    else
        OffsetRect(&circle->location, SQUARE_SIZE, 0);
}
void moveBottom(Circle *circle)
{
    if (circle->location.bottom >= GRID_LOCATION + GRID_SIZE)
        circle->direction ^= (TOP | BOTTOM);
    else
        OffsetRect(&circle->location, 0, SQUARE_SIZE);
}

void intersectCircle(Circle *, Circle *);

// NOTE tailCircle
// 따라가는 연산 -> 앞의 원이 있던 좌표를 그대로 물려받아서 출력
// BOOL followFlag 주인공 원을 따라가고 있는 아닌지 확인
// 꼬리원은 생성될 때마다 새로운 RECT 생성? -> 이러면 관리가 너무 힘들어짐
// 차라리 전체칸이 그렇게 많지 않으니까 그 칸만큼 미리 배열을 생성해서 그 안에 집어넣으면?
// struct _TailCircle
// {
//     BOOL followFlag;     연결 상태
//     int followCircle;    따라가는 앞의 원의 index, headCircle이면 -1
//     int nextCircleIdx       다음에 연결되어 올 원의 index, 없으면 -1
//     RECT location;       현재 위치
//     int moveType;        이동상태 (followFlag가 true면 작동 X)
//                            1. 좌우상하 한 방향
//                            2. 네모모양을 그리면서
//                            3. 제자리에 그대로
// };
// 근데 배열 인덱스를 통해 앞의 따라가는 원의 좌표를 받아와서 이동하려면
// 배열이 순차적이 아니라 연결된 순서대로 불려야 하는데 이건 어떻게 하지?
// -> 불러올 원의 index를 순서대로 저장하는 index 배열을 만들면? - headCircle에 대한 연결은 해결 가능
// -> 근데 주인공원 뿐만 아니라 꼬리원끼리 연결되면?
// -> 그렇다면 꼬리원 구조체에 뒤에 연결된 원의 idx를 가지면 연결리스트 마냥 계속 불러올 수 있지 않을까?
// 원이 생성된 전체 개수를 저장하는 카운트를 만들고 각 원에 뒤에 연결된 상태를 확인해서 불러옴
// 주인공원은 항상 먼저 그려지고 그 다음에 연결되어 있는 꼬리원들 그리기
//
// 원 구조체를 굳이 head, tail 분리할 필요 없을듯?

// NOTE draw
// 그리기를 굳이 불리하지 않고 이동과 동시에 레스터 연산 이용해서 바로 그려주기?
// 아니면 그냥 이동을 전부 끝낸 다음에 한 번에 다시 그려주기?
// -> 이건 근데 버퍼를 넣지 않는 이상 깜빡거리는건 그대로라서 그냥 아래로 구현하는게 편할듯?