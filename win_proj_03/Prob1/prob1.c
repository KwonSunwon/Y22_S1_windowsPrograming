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
    BOOL followFlag;     // false ���������� �̵�, true ���� ���� ����
    int followCircleIdx; // ���󰡴� ���� index

    int nextCircleIdx; // ������� ���� index

    RECT location;
    RECT prevLocation;
    int moveType; // -1 �̸� headCircle

    int direction;
    int squareMove;
    BOOL rowOrCol;
} Circle;

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Window Programming Lab";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

void drawGrid(HDC, RECT, int);
void drawCircle(HDC, Circle);
void moveTailCircle(Circle *, int); // ���󰡴� ���� �ҷ��� ���� ���� prevLocation �� �޾ƿ�, moveType�� ���� �̵���
void intersectCircle(Circle *, Circle *, int, int);

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
        RECT rcHead = {
            GRID_LOCATION,
            GRID_LOCATION,
            GRID_LOCATION + (GRID_SIZE / SQUARE_COUNT),
            GRID_LOCATION + (GRID_SIZE / SQUARE_COUNT)};
        RECT rcHead2 = {0, 0, 0, 0};
        headCircle.followFlag = FALSE;
        headCircle.followCircleIdx = -1;
        headCircle.moveType = -1;
        headCircle.nextCircleIdx = -1;
        headCircle.rowOrCol = TRUE;
        headCircle.direction = RIGHT | BOTTOM;
        headCircle.location = rcHead;
        headCircle.prevLocation = rcHead2;

        SetTimer(hWnd, 1, 100, NULL);
        SetTimer(hWnd, 2, 1000, NULL);
        // printf("%d, %d\n", headCircle.direction, RIGHT | BOTTOM);

        break;

    case WM_TIMER:
        switch (wParam)
        {
        case 1:
            moveHeadCircle(&headCircle);
            for (int i = 0; i < countTailCircle; ++i)
            {
                intersectCircle(&headCircle, &arrTailCircle[i], 0, i);
            }
            for (int i = 0; i < countTailCircle; ++i)
                moveTailCircle(&arrTailCircle[i], arrTailCircle->followCircleIdx);
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        case 2:
            srand(time(NULL));
            BOOL intersect = FALSE;
            do
            {
                arrTailCircle[countTailCircle].location.left = grid.left + (SQUARE_SIZE * (rand() % SQUARE_COUNT));
                arrTailCircle[countTailCircle].location.top = grid.top + (SQUARE_SIZE * (rand() % SQUARE_COUNT));
                arrTailCircle[countTailCircle].location.right = arrTailCircle[countTailCircle].location.left + SQUARE_SIZE;
                arrTailCircle[countTailCircle].location.bottom = arrTailCircle[countTailCircle].location.top + SQUARE_SIZE;
                if (countTailCircle > 0)
                {
                    RECT rcTemp;
                    for (int i = 0; i < countTailCircle; ++i)
                        if (IntersectRect(&rcTemp, &arrTailCircle[countTailCircle].location, &arrTailCircle[i].location) != 0)
                        {
                            // printf("��ħ\n");
                            intersect = TRUE;
                        }
                }
            } while (intersect);
            arrTailCircle[countTailCircle].followFlag = FALSE;
            arrTailCircle[countTailCircle].followCircleIdx = -1;
            arrTailCircle[countTailCircle].nextCircleIdx = -1;
            arrTailCircle[countTailCircle].moveType = rand() % 2;
            switch (rand() % 4)
            {
            case 0:
                arrTailCircle[countTailCircle].direction = LEFT;
                break;
            case 1:
                arrTailCircle[countTailCircle].direction = TOP;
                break;
            case 2:
                arrTailCircle[countTailCircle].direction = RIGHT;
                break;
            case 3:
                arrTailCircle[countTailCircle].direction = BOTTOM;
                break;
            }
            arrTailCircle[countTailCircle].squareMove = 5;
            ++countTailCircle;
            break;
        }
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

        for (int i = 0; i < countTailCircle; ++i)
        {
            drawCircle(hdc, arrTailCircle[i]);
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

void moveTailCircle(Circle *circle, int followIdx)
{
    if (circle->followFlag)
    {
        circle->prevLocation = circle->location;
        circle->location = circle[followIdx].prevLocation;
    }
    else
    {
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

    // printf("%d,%d,%d,%d\n", circle->location.left, circle->location.top, circle->location.right, circle->location.bottom);
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

void intersectCircle(Circle *circle, Circle *otherCircle, int idx, int otherIdx)
{
    RECT rcTemp;
    if (IntersectRect(&rcTemp, &circle->location, &otherCircle->location) != 0)
    {
        otherCircle->followFlag = TRUE;
        otherCircle->followCircleIdx = idx;

        circle->nextCircleIdx = otherIdx;
    }
}

// NOTE tailCircle
// ���󰡴� ���� -> ���� ���� �ִ� ��ǥ�� �״�� �����޾Ƽ� ���
// BOOL followFlag ���ΰ� ���� ���󰡰� �ִ� �ƴ��� Ȯ��
// �������� ������ ������ ���ο� RECT ����? -> �̷��� ������ �ʹ� �������
// ���� ��üĭ�� �׷��� ���� �����ϱ� �� ĭ��ŭ �̸� �迭�� �����ؼ� �� �ȿ� ���������?
// struct _TailCircle
// {
//     BOOL followFlag;     ���� ����
//     int followCircle;    ���󰡴� ���� ���� index, headCircle�̸� -1
//     int nextCircleIdx       ������ ����Ǿ� �� ���� index, ������ -1
//     RECT location;       ���� ��ġ
//     int moveType;        �̵����� (followFlag�� true�� �۵� X)
//                            1. �¿���� �� ����
//                            2. �׸����� �׸��鼭
//                            3. ���ڸ��� �״��
// };
// �ٵ� �迭 �ε����� ���� ���� ���󰡴� ���� ��ǥ�� �޾ƿͼ� �̵��Ϸ���
// �迭�� �������� �ƴ϶� ����� ������� �ҷ��� �ϴµ� �̰� ��� ����?
// -> �ҷ��� ���� index�� ������� �����ϴ� index �迭�� �����? - headCircle�� ���� ������ �ذ� ����
// -> �ٵ� ���ΰ��� �Ӹ� �ƴ϶� ���������� ����Ǹ�?
// -> �׷��ٸ� ������ ����ü�� �ڿ� ����� ���� idx�� ������ ���Ḯ��Ʈ ���� ��� �ҷ��� �� ���� ������?
// ���� ������ ��ü ������ �����ϴ� ī��Ʈ�� ����� �� ���� �ڿ� ����� ���¸� Ȯ���ؼ� �ҷ���
// ���ΰ����� �׻� ���� �׷����� �� ������ ����Ǿ� �ִ� �������� �׸���
//
// �� ����ü�� ���� head, tail �и��� �ʿ� ������?

// NOTE draw
// �׸��⸦ ���� �Ҹ����� �ʰ� �̵��� ���ÿ� ������ ���� �̿��ؼ� �ٷ� �׷��ֱ�?
// �ƴϸ� �׳� �̵��� ���� ���� ������ �� ���� �ٽ� �׷��ֱ�?
// -> �̰� �ٵ� ���۸� ���� �ʴ� �̻� �����Ÿ��°� �״�ζ� �׳� �Ʒ��� �����ϴ°� ���ҵ�?

// NOTE move & intersect
// �� ���� �̵��� ���� �� followFlag�� true�� ���¿����� ������
// ���� �ش� ���� ���󰡴� ���� idx�� ���߿� ������ ��, idx < followIdx �� followCircle�� location�� �޾Ƽ� �����ϰ�
// ���� �ش� ���� ���󰡴� ���� idx�� �տ� ���Դٸ� ��, idx > followIdx �� prevLocatoin�� ����
// �̷��� �ϸ� �������� �������� ���� �ȷο� idx�� ���� �θ� �ʿ���� �׳� 0 ���� count ���� �θ����