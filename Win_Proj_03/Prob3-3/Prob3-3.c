#define _CRT_SECURE_NO_WARNINGS
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include <stdio.h>

#include <Windows.h>
#include <tchar.h>

#define GRID_X_LOCATOIN 60
#define GRID_Y_LOCATION 30
#define GRID_X_SIZE 420
#define GRID_Y_SIZE 720

#define DOWN_TIMER 1

#define YELLOW RGB(255, 255, 0)
#define GREEN RGB(0, 220, 0)
#define CYAN RGB(0, 220, 220)

static int popCount;

typedef struct _Block
{
    RECT location;
    COLORREF color;
    BOOL isExist;
} Block;

typedef struct _Queue
{
    POINT pt[100];
    int rear;
} Queue;

void drawGrid(HDC, RECT, int, int);
void drawBlock(HDC, Block);
void drawBack(HDC, Block[][6]);

BOOL checkDown(Block *, int *, Block[][6], RECT);

BOOL checkPop(Block[][6]);
void checkDrop(Block[][6]);

BOOL bfs(int, int, Block[][6], int[][6]);
void popPossible(int, int, Block[][6], BOOL[][6], Queue *);

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

    hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 600, 800, NULL, (HMENU)NULL, hInstance, NULL);
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

    RECT grid = {
        GRID_X_LOCATOIN,
        GRID_Y_LOCATION,
        GRID_X_LOCATOIN + GRID_X_SIZE,
        GRID_Y_LOCATION + GRID_Y_SIZE,
    };

    static Block downBlock[2];
    static BOOL isExist;
    static int select;

    static Block backBlock[12][6];

    static BOOL s;

    RECT rcTmp;

    switch (iMessage)
    {
    case WM_CREATE:
        rcTmp.left = grid.left;
        rcTmp.top = grid.bottom - 60;
        rcTmp.right = rcTmp.left + 70;
        rcTmp.bottom = grid.bottom;

        for (int i = 0; i < 12; ++i)
        {
            for (int j = 0; j < 6; ++j)
            {
                rcTmp.left = grid.left + 70 * j;
                rcTmp.top = grid.bottom - 60 * (i + 1);
                rcTmp.right = rcTmp.left + 70;
                rcTmp.bottom = rcTmp.top + 60;

                backBlock[i][j].location = rcTmp;
                backBlock[i][j].isExist = FALSE;

                // printf("%d, %d - %d, %d, %d, %d\n", i, j, backBlock[i][j].location.left, backBlock[i][j].location.top, backBlock[i][j].location.right, backBlock[i][j].location.bottom);
            }
        }

        isExist = FALSE;
        SetTimer(hWnd, DOWN_TIMER, 700, NULL);

        break;

    case WM_TIMER:
        switch (wParam)
        {
        case DOWN_TIMER:
            if (!isExist)
            {
                // if (1)
                if (rand() % 2)
                {
                    switch (rand() % 3)
                    {
                    case 0:
                        downBlock[0].color = YELLOW;
                        break;
                    case 1:
                        downBlock[0].color = GREEN;
                        break;
                    case 2:
                        downBlock[0].color = CYAN;
                        break;
                    }

                    downBlock[0].location.left = GRID_X_LOCATOIN + 70 * 2;
                    downBlock[0].location.top = GRID_Y_LOCATION;
                    downBlock[0].location.right = GRID_X_LOCATOIN + 70 * 3;
                    downBlock[0].location.bottom = GRID_Y_LOCATION + 60;

                    select = 1;
                }
                else
                {
                    switch (rand() % 3)
                    {
                    case 0:
                        downBlock[0].color = YELLOW;
                        downBlock[1].color = YELLOW;
                        break;
                    case 1:
                        downBlock[0].color = GREEN;
                        downBlock[1].color = GREEN;
                        break;
                    case 2:
                        downBlock[0].color = CYAN;
                        downBlock[1].color = CYAN;
                        break;
                    }

                    downBlock[0].location.left = GRID_X_LOCATOIN + 70 * 2;
                    downBlock[0].location.top = GRID_Y_LOCATION;
                    downBlock[0].location.right = GRID_X_LOCATOIN + 70 * 3;
                    downBlock[0].location.bottom = GRID_Y_LOCATION + 60;

                    // 서 있는 모양
                    if (rand() % 2)
                    {
                        downBlock[1].location.left = GRID_X_LOCATOIN + 70 * 2;
                        downBlock[1].location.top = GRID_Y_LOCATION + 60;
                        downBlock[1].location.right = GRID_X_LOCATOIN + 70 * 3;
                        downBlock[1].location.bottom = GRID_Y_LOCATION + 60 * 2;
                        select = 2;
                    }
                    // 누운 모양
                    else
                    {
                        downBlock[1].location.left = GRID_X_LOCATOIN + 70 * 3;
                        downBlock[1].location.top = GRID_Y_LOCATION;
                        downBlock[1].location.right = GRID_X_LOCATOIN + 70 * 4;
                        downBlock[1].location.bottom = GRID_Y_LOCATION + 60;
                        select = 3;
                    }
                }

                isExist = TRUE;
            }
            else
            {
                if (select == 1)
                {
                    OffsetRect(&downBlock[0].location, 0, 60);
                    if (checkDown(downBlock, &select, backBlock, grid))
                    {
                        select = 0;
                        isExist = FALSE;
                        while (checkPop(backBlock))
                        {
                            checkDrop(backBlock);
                        }
                    }
                }
                else if (select == 2 || select == 3)
                {
                    OffsetRect(&downBlock[0].location, 0, 60);
                    OffsetRect(&downBlock[1].location, 0, 60);
                    if (checkDown(downBlock, &select, backBlock, grid))
                    {
                        select = 0;
                        isExist = FALSE;
                        while (checkPop(backBlock))
                        {
                            checkDrop(backBlock);
                        }
                    }
                }
            }
            break;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        // 사각형 회전
        case VK_RETURN:
            if (select == 2)
            {
                OffsetRect(&downBlock[1].location, 70, -60);
                select = 3;
            }
            else if (select == 3)
            {
                OffsetRect(&downBlock[1].location, -70, 60);
                select = 2;
            }
            InvalidateRect(hWnd, NULL, FALSE);
            break;

        // 좌우 이동
        case VK_LEFT:
            if (select == 1)
                OffsetRect(&downBlock[0].location, -70, 0);
            else if (select == 2 || select == 3)
            {
                OffsetRect(&downBlock[0].location, -70, 0);
                OffsetRect(&downBlock[1].location, -70, 0);
            }
            InvalidateRect(hWnd, NULL, FALSE);
            break;

        case VK_RIGHT:
            if (select == 1)
                OffsetRect(&downBlock[0].location, 70, 0);
            else if (select == 2 || select == 3)
            {
                OffsetRect(&downBlock[0].location, 70, 0);
                OffsetRect(&downBlock[1].location, 70, 0);
            }
            InvalidateRect(hWnd, NULL, FALSE);
            break;

        // 떨어지는 속도 빠르게
        case VK_DOWN:
            if (select == 1)
            {
                OffsetRect(&downBlock[0].location, 0, 60);
                if (checkDown(downBlock, &select, backBlock, grid))
                {
                    select = 0;
                    isExist = FALSE;
                    while (checkPop(backBlock))
                    {
                        checkDrop(backBlock);
                    }
                }
            }
            else if (select == 2 || select == 3)
            {
                OffsetRect(&downBlock[0].location, 0, 60);
                OffsetRect(&downBlock[1].location, 0, 60);
                if (checkDown(downBlock, &select, backBlock, grid))
                {
                    select = 0;
                    isExist = FALSE;
                    while (checkPop(backBlock))
                    {
                        checkDrop(backBlock);
                    }
                }
            }
            InvalidateRect(hWnd, NULL, FALSE);
            break;

        case 0x53:
            if (s)
            {
                SetTimer(hWnd, DOWN_TIMER, 1000, NULL);
                s = FALSE;
            }
            else
            {
                KillTimer(hWnd, DOWN_TIMER);
                s = TRUE;
            }
            break;
        }
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

        // Draw, using hdc
        drawGrid(hdc, grid, 12, 6);

        if (isExist)
        {
            if (select == 1)
                drawBlock(hdc, downBlock[0]);
            else if (select == 2 || select == 3)
            {
                drawBlock(hdc, downBlock[0]);
                drawBlock(hdc, downBlock[1]);
            }
        }

        drawBack(hdc, backBlock);

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

void drawGrid(HDC hdc, RECT rt, int row, int col)
{
    FrameRect(hdc, &rt, (HPEN)CreatePen(PS_SOLID, 1, BLACK_PEN));

    int rowSize = (rt.bottom - rt.top) / row;
    int colSize = (rt.right - rt.left) / col;

    for (int i = 1; i < row; ++i)
    {
        MoveToEx(hdc, rt.left, rt.top + rowSize * i, NULL);
        LineTo(hdc, rt.right, rt.top + rowSize * i, NULL);
    }
    for (int i = 1; i < col; ++i)
    {
        MoveToEx(hdc, rt.left + colSize * i, rt.top, NULL);
        LineTo(hdc, rt.left + colSize * i, rt.bottom, NULL);
    }
}

void drawBlock(HDC hdc, Block block)
{
    HBRUSH hBrush, oldBrush;

    hBrush = CreateSolidBrush(block.color);
    oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    RECT rt = block.location;
    Rectangle(hdc, rt.left, rt.top, rt.right, rt.bottom);

    SelectObject(hdc, oldBrush);
    DeleteObject(hBrush);
}

BOOL checkDown(Block *block, int *select, Block back[][6], RECT grid)
{
    RECT rcTmp;

    int sel = *select;

    if (sel == 1)
    {
        if (block[0].location.bottom > grid.bottom)
        {
            OffsetRect(&block[0].location, 0, -60);
            for (int i = 0; i < 6; ++i)
            {
                if (IntersectRect(&rcTmp, &block[0].location, &back[0][i].location))
                {
                    back[0][i].isExist = TRUE;
                    back[0][i].color = block[0].color;
                    return TRUE;
                }
            }
        }
        else
        {
            // printf("block %d, %d, %d, %d\n", block[0].location.left, block[0].location.top, block[0].location.right, block[0].location.bottom);
            for (int i = 0; i < 12; ++i)
                for (int j = 0; j < 6; ++j)
                {
                    if (IntersectRect(&rcTmp, &block[0].location, &back[i][j].location) && back[i][j].isExist)
                    {
                        // printf("back %d, %d, %d, %d\n", back[i][j].location.left, back[i][j].location.top, back[i][j].location.right, back[i][j].location.bottom);
                        // printf("진입4\n");
                        back[i + 1][j].isExist = TRUE;
                        back[i + 1][j].color = block[0].color;
                        return TRUE;
                    }
                }
        }
    }
    else if (sel == 2) // 서 있는 모양
    {
        if (block[1].location.bottom > grid.bottom)
        {
            OffsetRect(&block[0].location, 0, -60);
            OffsetRect(&block[1].location, 0, -60);
            for (int i = 0; i < 6; ++i)
            {
                if (IntersectRect(&rcTmp, &block[1].location, &back[1][i].location))
                {
                    back[0][i].isExist = TRUE;
                    back[0][i].color = block[1].color;
                    back[1][i].isExist = TRUE;
                    back[1][i].color = block[0].color;
                    return TRUE;
                }
            }
        }
        else
        {
            for (int i = 0; i < 11; ++i)
                for (int j = 0; j < 6; ++j)
                {
                    if (IntersectRect(&rcTmp, &block[1].location, &back[i][j].location) && back[i][j].isExist)
                    {
                        back[i + 1][j].isExist = TRUE;
                        back[i + 1][j].color = block[1].color;
                        back[i + 2][j].isExist = TRUE;
                        back[i + 2][j].color = block[0].color;
                        return TRUE;
                    }
                }
        }
    }
    else if (sel == 3)
    {
        if (block[0].location.bottom > grid.bottom)
        {
            OffsetRect(&block[0].location, 0, -60);
            OffsetRect(&block[1].location, 0, -60);
            for (int i = 0; i < 5; ++i)
            {
                if (IntersectRect(&rcTmp, &block[0].location, &back[0][i].location))
                {
                    back[0][i].isExist = TRUE;
                    back[0][i].color = block[0].color;
                    back[0][i + 1].isExist = TRUE;
                    back[0][i + 1].color = block[1].color;
                    return TRUE;
                }
            }
        }
        else
        {
            for (int i = 0; i < 12; ++i)
                for (int j = 0; j < 6; ++j)
                {
                    if (IntersectRect(&rcTmp, &block[0].location, &back[i][j].location) && back[i][j].isExist)
                    {
                        back[i + 1][j].isExist = TRUE;
                        back[i + 1][j].color = block[0].color;

                        if (IntersectRect(&rcTmp, &block[1].location, &back[i][j + 1].location) && back[i][j + 1].isExist)
                        {
                            back[i + 1][j + 1].isExist = TRUE;
                            back[i + 1][j + 1].color = block[1].color;
                            return TRUE;
                        }
                        else
                        {
                            *select = 1;
                            block[0] = block[1];
                            return FALSE;
                        }
                    }
                    if (IntersectRect(&rcTmp, &block[1].location, &back[i][j].location) && back[i][j].isExist)
                    {
                        back[i + 1][j].isExist = TRUE;
                        back[i + 1][j].color = block[1].color;

                        if (IntersectRect(&rcTmp, &block[0].location, &back[i][j - 1].location) && back[i][j - 1].isExist)
                        {
                            back[i + 1][j - 1].isExist = TRUE;
                            back[i + 1][j - 1].color = block[0].color;
                            return TRUE;
                        }
                        else
                        {
                            *select = 1;
                            block[1] = block[0];
                            return FALSE;
                        }
                    }
                }
        }
    }

    return FALSE;
}

void drawBack(HDC hdc, Block back[][6])
{
    HBRUSH hBrush, oldBrush;

    for (int i = 0; i < 12; ++i)
        for (int j = 0; j < 6; ++j)
            if (back[i][j].isExist)
            {
                hBrush = CreateSolidBrush(back[i][j].color);
                oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
                RECT rt = back[i][j].location;
                Rectangle(hdc, rt.left, rt.top, rt.right, rt.bottom);
                SelectObject(hdc, oldBrush);
                DeleteObject(hBrush);
            }
}

BOOL checkPop(Block back[][6])
{
    BOOL isPop = FALSE;

    Queue q;

    for (int i = 0; i < 12; ++i)
        for (int j = 0; j < 6; ++j)
        {
            q.rear = -1;
            popCount = 1;
            BOOL visit[12][6] = {FALSE};

            if (back[i][j].isExist)
            {
                q.pt[++q.rear] = (POINT){i, j};
                popPossible(i, j, back, visit, &q);
                printf("%d\n", popCount);
            }

            if (popCount >= 3)
            {
                printf("제거\n");
                for (int k = 0; k < popCount; ++k)
                    back[q.pt[k].x][q.pt[k].y].isExist = FALSE;
                isPop = TRUE;
            }
        }

    return isPop;
}

void popPossible(int x, int y, Block back[][6], BOOL visit[][6], Queue *q)
{
    COLORREF color = back[x][y].color;
    visit[x][y] = TRUE;
    if (y - 1 > 0)
        if (back[x][y - 1].isExist && !visit[x][y - 1])
            if (color == back[x][y - 1].color)
            {
                popCount++;
                q->pt[++q->rear] = (POINT){x, y - 1};
                popPossible(x, y - 1, back, visit, q);
            }
    if (y + 1 < 6)
        if (back[x][y + 1].isExist && !visit[x][y + 1])
            if (color == back[x][y + 1].color)
            {
                popCount++;
                q->pt[++q->rear] = (POINT){x, y + 1};

                popPossible(x, y + 1, back, visit, q);
            }
    if (x - 1 > 0)
        if (back[x - 1][y].isExist && !visit[x - 1][y])
            if (color == back[x - 1][y].color)
            {
                popCount++;
                q->pt[++q->rear] = (POINT){x - 1, y};

                popPossible(x - 1, y, back, visit, q);
            }
    if (x + 1 < 12)
        if (back[x + 1][y].isExist && !visit[x + 1][y])
            if (color == back[x + 1][y].color)
            {
                popCount++;
                q->pt[++q->rear] = (POINT){x + 1, y};
                popPossible(x + 1, y, back, visit, q);
            }
}

void checkDrop(Block back[][6])
{
    for (int i = 1; i < 12; ++i)
        for (int j = 0; j < 6; ++j)
        {
            int k = 0;
            while (1)
            {
                if (back[i - k][j].isExist)
                {
                    if (!back[i - (k + 1)][j].isExist)
                    {
                        back[i - (k + 1)][j] = back[i - k][j];
                        back[i - k][j].isExist = FALSE;
                    }
                    else
                        break;
                }
                else
                    break;

                if (i - k < 0)
                    break;
                k++;
            }
        }
}

BOOL bfs(int x, int y, Block back[][6], int visit[][6])
{
    BOOL isPop = FALSE;
    int count = 1;

    int dx[4] = {1, -1, 0, 0};
    int dy[4] = {0, 0, 1, -1};

    POINT queue[100];
    int front = -1;
    int rear = -1;

    visit[x][y] = 1;

    queue[++rear] = (POINT){x, y};

    while (front != rear)
    {
        POINT pt = queue[++front];

        for (int i = 0; i < 4; ++i)
        {
            POINT pt2;
            pt2.x = pt.x + dx[i];
            pt2.y = pt.y + dy[i];

            if (pt2.x < 0 || pt2.x >= 12 || pt2.y < 0 || pt2.y >= 6 || visit[pt2.x][pt2.y])
                continue;

            if (back[pt2.x][pt2.y].color == back[x][y].color && back[pt2.x][pt2.y].isExist)
            {
                queue[++rear] = pt2;
                ++count;
            }
        }
    }

    if (count >= 3)
    {
        for (int i = 0; i <= rear; ++i)
            back[queue[i].x][queue[i].y].isExist = FALSE;
        isPop = TRUE;
    }
    return isPop;
}
