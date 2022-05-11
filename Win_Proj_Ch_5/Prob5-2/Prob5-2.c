#define _CRT_SECURE_NO_WARNINGS
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include <stdio.h>

#include <Windows.h>
#include <tchar.h>
#include <time.h>

#include "resource.h"

#define TIME_TILE_MOVE 1
#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3

void image_divide(BITMAP, int, RECT[][5], POINT *, BOOL[][5]);
void shuffle(POINT *, int);
POINT search_move_tile(BOOL *, int, int);

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
    HDC hdc, bitdc;
    HBITMAP oldBitmap;
    static HDC mdc;
    static HBITMAP backBit;
    static RECT bufferRT;

    static HBITMAP image[2];
    static BITMAP imageInfo[2];
    static int imageSelect;

    static BOOL isGameStart;
    static BOOL isFullImage;
    static BOOL isInversion;

    static int divideBy;
    static RECT divideImageRT[5][5];

    static POINT imagePT[25];

    static POINT firstPT;
    static POINT lastPT;
    static BOOL lbClick;

    static POINT movingTile;
    static BOOL isMoving;
    static int remainMove;
    static BOOL divide[5][5];
    static int direction;

    static BOOL isMoveEnough;

    int mx, my;
    double width, height;
    DWORD dword;

    static int tmp1, tmp2;

    switch (iMessage)
    {
    case WM_CREATE:
        image[0] = (HBITMAP)LoadImage(NULL, L"image1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
        image[1] = (HBITMAP)LoadImage(NULL, L"image2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
        GetObject(image[0], sizeof(BITMAP), &imageInfo[0]);
        GetObject(image[1], sizeof(BITMAP), &imageInfo[1]);

        imageSelect = 0;
        isGameStart = FALSE;
        isFullImage = FALSE;
        isInversion = FALSE;

        lbClick = FALSE;

        divideBy = 3;

        image_divide(imageInfo[0], divideBy, divideImageRT, imagePT, divide);
        break;

    case WM_TIMER:
        switch (wParam)
        {
        case TIME_TILE_MOVE:
            remainMove -= 10;
            if (remainMove <= 0)
            {
                POINT tmpPT = imagePT[tmp1];
                imagePT[tmp1] = imagePT[tmp2];
                imagePT[tmp2] = tmpPT;
                isMoving = FALSE;
                KillTimer(hWnd, TIME_TILE_MOVE);
            }
            break;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_COMMAND:
        switch (wParam)
        {
        case ID_SELECT_1:
            if (!isGameStart)
            {
                imageSelect = 0;
                image_divide(imageInfo[0], divideBy, divideImageRT, imagePT, divide);
            }
            break;

        case ID_SELECT_2:
            if (!isGameStart)
            {
                imageSelect = 1;
                image_divide(imageInfo[0], divideBy, divideImageRT, imagePT, divide);
            }
            break;

        case ID_DIVIDE_3:
            if (!isGameStart)
            {
                divideBy = 3;
                image_divide(imageInfo[imageSelect], divideBy, divideImageRT, imagePT, divide);
            }
            break;

        case ID_DIVIDE_4:
            if (!isGameStart)
            {
                divideBy = 4;
                image_divide(imageInfo[imageSelect], divideBy, divideImageRT, imagePT, divide);
            }
            break;

        case ID_DIVIDE_5:
            if (!isGameStart)
            {
                divideBy = 5;
                image_divide(imageInfo[imageSelect], divideBy, divideImageRT, imagePT, divide);
            }
            break;

        case ID_GAME_START:
            isGameStart = TRUE;
            break;

        case ID_GAME_FULLIMAGE:
            if (!isFullImage)
                isFullImage = TRUE;
            else
                isFullImage = FALSE;
            break;

        case ID_GAME_INVERSION:
            if (!isInversion)
                isInversion = TRUE;
            else
                isInversion = FALSE;
            break;

        case ID_GAME_STOP:
            isGameStart = FALSE;
            break;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case 0x53: // S ���� ����
            isGameStart = TRUE;
            break;

        case 0x46: // F ��ü �׸� ����
            if (!isFullImage)
                isFullImage = TRUE;
            else
                isFullImage = FALSE;
            break;

        case 0x51: // Q ���� ����
            isGameStart = FALSE;
            break;

        case 0x31: // 1 1�� ���� ����
            if (!isGameStart)
            {
                imageSelect = 0;
                image_divide(imageInfo[0], divideBy, divideImageRT, imagePT, divide);
            }
            break;

        case 0x32: // 2 2�� ���� ����
            if (!isGameStart)
            {
                imageSelect = 1;
                image_divide(imageInfo[1], divideBy, divideImageRT, imagePT, divide);
            }
            break;

        case 0x33: // 3 3x3 �׸� ������
            if (!isGameStart)
            {
                divideBy = 3;
                image_divide(imageInfo[imageSelect], divideBy, divideImageRT, imagePT, divide);
            }
            break;

        case 0x34: // 4 4x4 �׸� ������
            if (!isGameStart)
            {
                divideBy = 4;
                image_divide(imageInfo[imageSelect], divideBy, divideImageRT, imagePT, divide);
            }
            break;

        case 0x35: // 5 5x5 �׸� ������
            if (!isGameStart)
            {
                divideBy = 5;
                image_divide(imageInfo[imageSelect], divideBy, divideImageRT, imagePT, divide);
            }
            break;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_LBUTTONDOWN:
        lbClick = TRUE;
        mx = LOWORD(lParam);
        my = HIWORD(lParam);
        firstPT = (POINT){mx, my};
        break;

    case WM_MOUSEMOVE:
        if (lbClick && !isMoving)
        {
            mx = LOWORD(lParam);
            my = HIWORD(lParam);
            lastPT = (POINT){mx, my};

            if (abs(firstPT.x - lastPT.x) > 80 || abs(firstPT.y - lastPT.y) > 80)
            {
                if (abs(firstPT.x - lastPT.x) > abs(firstPT.y - lastPT.y))
                {
                    if (firstPT.x > lastPT.x)
                    {
                        // �������� �巡�� ������ ���� ����(�� ĭ)���� �̵� == �� ĭ�� ���������� �̵�
                        direction = LEFT;
                        isMoveEnough = TRUE;
                        printf("left\n");
                    }
                    else
                    {
                        // ���������� �巡�� ���� ���� ������(�� ĭ)���� �̵�
                        direction = RIGHT;
                        isMoveEnough = TRUE;
                        printf("right\n");
                    }
                }
                else
                {
                    if (firstPT.y > lastPT.y)
                    {
                        // �������� �巡�� �Ʒ��� ���� ����(�� ĭ)���� �̵�
                        direction = UP;
                        isMoveEnough = TRUE;
                        printf("up\n");
                    }
                    else
                    {
                        // �Ʒ������� �巡�� ���� ���� �Ʒ���(�� ĭ)���� �̵�
                        direction = DOWN;
                        isMoveEnough = TRUE;
                        printf("down\n");
                    }
                }
            }
        }
        break;

    case WM_LBUTTONUP:
        lbClick = FALSE;
        if (isMoveEnough)
        {
            movingTile = search_move_tile(divide, divideBy, direction);
            if (movingTile.x != -1 && movingTile.y != -1)
            {
                // printf("%d%d %d%d %d%d\n", imagePT[0].x, imagePT[0].y, imagePT[1].x, imagePT[1].y, imagePT[2].x, imagePT[2].y);
                // printf("%d%d %d%d %d%d\n", imagePT[3].x, imagePT[3].y, imagePT[4].x, imagePT[4].y, imagePT[5].x, imagePT[5].y);
                // printf("%d%d %d%d %d%d\n", imagePT[6].x, imagePT[6].y, imagePT[7].x, imagePT[7].y, imagePT[8].x, imagePT[8].y);

                int idx = 0;
                for (int i = 0; i < divideBy; ++i)
                    for (int j = 0; j < divideBy; ++j)
                    {
                        if (movingTile.x == i && movingTile.y == j)
                        {
                            tmp1 = idx;
                        }
                        if (imagePT[idx].x == divideBy - 1 && imagePT[idx].y == divideBy - 1)
                        {
                            tmp2 = idx;
                        }
                        idx++;
                    }

                movingTile = imagePT[tmp1];

                remainMove = divideImageRT[0][0].right;

                isMoving = TRUE;

                SetTimer(hWnd, TIME_TILE_MOVE, 30, NULL);
            }
            isMoveEnough = FALSE;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &bufferRT);
        mdc = CreateCompatibleDC(hdc);
        backBit = CreateCompatibleBitmap(hdc, bufferRT.right, bufferRT.bottom);
        SelectObject(mdc, (HBITMAP)backBit);

        if (isInversion)
        {
            dword = NOTSRCCOPY;
            PatBlt(mdc, 0, 0, bufferRT.right, bufferRT.bottom, WHITENESS);
        }
        else
        {
            dword = SRCCOPY;
            PatBlt(mdc, 0, 0, bufferRT.right, bufferRT.bottom, BLACKNESS);
        }

        bitdc = CreateCompatibleDC(hdc);
        oldBitmap = SelectObject(bitdc, (HBITMAP)image[imageSelect]);

        width = bufferRT.right / divideBy;
        height = bufferRT.bottom / divideBy;

        if (!isFullImage)
        {
            int idx = 0;
            for (int i = 0; i < divideBy; ++i)
                for (int j = 0; j < divideBy; ++j)
                {
                    POINT pt = imagePT[idx++];
                    if (pt.x == (divideBy - 1) && pt.y == (divideBy - 1))
                        continue;

                    if (movingTile.x == pt.x && movingTile.y == pt.y && isMoving)
                    {
                        switch (direction)
                        {
                        case LEFT:
                            StretchBlt(mdc, width * j - (width - remainMove), height * i, width, height,
                                       bitdc, divideImageRT[pt.x][pt.y].left, divideImageRT[pt.x][pt.y].top,
                                       divideImageRT[pt.x][pt.y].right, divideImageRT[pt.x][pt.y].bottom, dword);
                            break;
                        case RIGHT:
                            StretchBlt(mdc, width * j + (width - remainMove), height * i, width, height,
                                       bitdc, divideImageRT[pt.x][pt.y].left, divideImageRT[pt.x][pt.y].top,
                                       divideImageRT[pt.x][pt.y].right, divideImageRT[pt.x][pt.y].bottom, dword);
                            break;
                        case UP:
                            StretchBlt(mdc, width * j, height * i - (height - remainMove), width, height,
                                       bitdc, divideImageRT[pt.x][pt.y].left, divideImageRT[pt.x][pt.y].top,
                                       divideImageRT[pt.x][pt.y].right, divideImageRT[pt.x][pt.y].bottom, dword);
                            break;
                        case DOWN:
                            StretchBlt(mdc, width * j, height * i + (height - remainMove), width, height,
                                       bitdc, divideImageRT[pt.x][pt.y].left, divideImageRT[pt.x][pt.y].top,
                                       divideImageRT[pt.x][pt.y].right, divideImageRT[pt.x][pt.y].bottom, dword);
                            break;
                        }
                    }
                    else
                        StretchBlt(mdc, width * j, height * i, width, height,
                                   bitdc, divideImageRT[pt.x][pt.y].left, divideImageRT[pt.x][pt.y].top,
                                   divideImageRT[pt.x][pt.y].right, divideImageRT[pt.x][pt.y].bottom, dword);
                }
        }
        else
        {
            for (int i = 0; i < divideBy; ++i)
                for (int j = 0; j < divideBy; ++j)
                {
                    StretchBlt(mdc, width * i, height * j, width, height,
                               bitdc, divideImageRT[i][j].left, divideImageRT[i][j].top,
                               divideImageRT[i][j].right, divideImageRT[i][j].bottom, dword);
                }
        }

        // for (int i = 0; i < divideBy; ++i)
        // {
        //     for (int j = 0; j < divideBy; ++j)
        //     {
        //         if (divide[i][j])
        //             printf("1 ");
        //         else
        //             printf("0 ");
        //     }
        //     printf("\n");
        // }
        // printf("%d%d %d%d %d%d\n", imagePT[0].x, imagePT[0].y, imagePT[1].x, imagePT[1].y, imagePT[2].x, imagePT[2].y);
        // printf("%d%d %d%d %d%d\n", imagePT[3].x, imagePT[3].y, imagePT[4].x, imagePT[4].y, imagePT[5].x, imagePT[5].y);
        // printf("%d%d %d%d %d%d\n", imagePT[6].x, imagePT[6].y, imagePT[7].x, imagePT[7].y, imagePT[8].x, imagePT[8].y);

        SelectObject(bitdc, oldBitmap);
        DeleteDC(bitdc);

        GetClientRect(hWnd, &bufferRT);
        BitBlt(hdc, 0, 0, bufferRT.right, bufferRT.bottom, mdc, 0, 0, SRCCOPY);
        DeleteObject(backBit);
        DeleteDC(mdc);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        DeleteObject(image[0]);
        DeleteObject(image[1]);
        PostQuitMessage(0);
        return 0;
    }
    return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void image_divide(BITMAP imageInfo, int divideBy, RECT divideRT[][5], POINT *imagePT, BOOL divide[][5])
{
    double width = imageInfo.bmWidth / divideBy;
    double height = imageInfo.bmHeight / divideBy;

    int idx = 0;

    for (int i = 0; i < divideBy; ++i)
        for (int j = 0; j < divideBy; ++j)
        {
            // �� �̹��� ��ǥ
            divideRT[i][j].left = width * i;
            divideRT[i][j].top = height * j;

            // �� �̹��� ���� ũ��
            divideRT[i][j].right = width;
            divideRT[i][j].bottom = height;

            imagePT[idx].x = i;
            imagePT[idx].y = j;
            printf("%d%d ", imagePT[idx].x, imagePT[idx].y);
            ++idx;
        }
    printf("\n");

    shuffle(imagePT, divideBy * divideBy);

    idx = 0;
    for (int i = 0; i < divideBy; ++i)
        for (int j = 0; j < divideBy; ++j)
        {
            if (imagePT[idx].x == divideBy - 1 && imagePT[idx].y == divideBy - 1)
                divide[i][j] = TRUE;
            else
                divide[i][j] = FALSE;
            printf("%d%d ", imagePT[idx].x, imagePT[idx].y);
            ++idx;
        }
    printf("\n");
}

void shuffle(POINT *arr, int num)
{
    srand(time(NULL));
    POINT temp;
    int rn;
    for (int i = 0; i < num - 1; i++)
    {
        rn = rand() % (num - i) + i;
        temp = arr[i];
        arr[i] = arr[rn];
        arr[rn] = temp;
    }
}

POINT search_move_tile(BOOL divide[][5], int divideBy, int direction)
{
    POINT moveTile;
    POINT lastTile;
    for (int i = 0; i < divideBy; ++i)
        for (int j = 0; j < divideBy; ++j)
            if (divide[i][j])
            {
                lastTile.x = i;
                lastTile.y = j;
            }

    printf("lastTile %d, %d\n", lastTile.x, lastTile.y);

    switch (direction)
    {
    case LEFT:
        if (lastTile.y == divideBy - 1)
        {
            moveTile = (POINT){-1, -1};
        }
        else
        {
            moveTile = (POINT){lastTile.x, lastTile.y + 1};
            divide[lastTile.x][lastTile.y] = FALSE;
            divide[moveTile.x][moveTile.y] = TRUE;
        }
        break;

    case RIGHT:
        if (lastTile.y == 0)
        {
            moveTile = (POINT){-1, -1};
        }
        else
        {
            moveTile = (POINT){lastTile.x, lastTile.y - 1};
            divide[lastTile.x][lastTile.y] = FALSE;
            divide[moveTile.x][moveTile.y] = TRUE;
        }
        break;

    case UP:
        if (lastTile.x == divideBy - 1)
        {
            moveTile = (POINT){-1, -1};
        }
        else
        {
            moveTile = (POINT){lastTile.x + 1, lastTile.y};
            divide[lastTile.x][lastTile.y] = FALSE;
            divide[moveTile.x][moveTile.y] = TRUE;
        }
        break;

    case DOWN:
        if (lastTile.x == 0)
        {
            moveTile = (POINT){-1, -1};
        }
        else
        {
            moveTile = (POINT){lastTile.x - 1, lastTile.y};
            divide[lastTile.x][lastTile.y] = FALSE;
            divide[moveTile.x][moveTile.y] = TRUE;
        }
        break;
    }

    printf("moveTile %d, %d\n", moveTile.x, moveTile.y);

    return moveTile;
}