#define _CRT_SECURE_NO_WARNINGS
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include <stdio.h>

#include <Windows.h>
#include <tchar.h>
#include <time.h>

#include "resource.h"

#define TIME_TILE_MOVE 1

void image_divide(BITMAP, int, RECT[][5], POINT *, BOOL *);
void shuffle(POINT *, int);

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

    int mx, my;
    double width, height;
    DWORD dword;

    static BOOL divide3[3][3];
    static BOOL divide4[4][4];
    static BOOL divide5[5][5];

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

        image_divide(imageInfo[0], divideBy, divideImageRT, imagePT, (BOOL *)divide3);

        break;

    case WM_TIMER:
        switch (lParam)
        {
        case TIME_TILE_MOVE:
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
                switch (divideBy)
                {
                case 3:
                    image_divide(imageInfo[0], divideBy, divideImageRT, imagePT, (BOOL *)divide3);
                    break;
                case 4:
                    image_divide(imageInfo[0], divideBy, divideImageRT, imagePT, (BOOL *)divide4);
                    break;
                case 5:
                    image_divide(imageInfo[0], divideBy, divideImageRT, imagePT, (BOOL *)divide5);
                    break;
                }
            }
            break;

        case ID_SELECT_2:
            if (!isGameStart)
            {
                imageSelect = 1;
                switch (divideBy)
                {
                case 3:
                    image_divide(imageInfo[1], divideBy, divideImageRT, imagePT, (BOOL *)divide3);
                    break;
                case 4:
                    image_divide(imageInfo[1], divideBy, divideImageRT, imagePT, (BOOL *)divide4);
                    break;
                case 5:
                    image_divide(imageInfo[1], divideBy, divideImageRT, imagePT, (BOOL *)divide5);
                    break;
                }
            }
            break;

        case ID_DIVIDE_3:
            if (!isGameStart)
            {
                divideBy = 3;
                image_divide(imageInfo[imageSelect], divideBy, divideImageRT, imagePT, (BOOL *)divide3);
            }
            break;

        case ID_DIVIDE_4:
            if (!isGameStart)
            {
                divideBy = 4;
                image_divide(imageInfo[imageSelect], divideBy, divideImageRT, imagePT, (BOOL *)divide4);
            }
            break;

        case ID_DIVIDE_5:
            if (!isGameStart)
            {
                divideBy = 5;
                image_divide(imageInfo[imageSelect], divideBy, divideImageRT, imagePT, (BOOL *)divide5);
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
        case 0x53: // S 게임 시작
            isGameStart = TRUE;
            break;

        case 0x46: // F 전체 그림 보기
            if (!isFullImage)
                isFullImage = TRUE;
            else
                isFullImage = FALSE;
            break;

        case 0x51: // Q 게임 종료
            isGameStart = FALSE;
            break;

        case 0x31: // 1 1번 사진 선택
            if (!isGameStart)
            {
                imageSelect = 0;
                switch (divideBy)
                {
                case 3:
                    image_divide(imageInfo[0], divideBy, divideImageRT, imagePT, (BOOL *)divide3);
                    break;
                case 4:
                    image_divide(imageInfo[0], divideBy, divideImageRT, imagePT, (BOOL *)divide4);
                    break;
                case 5:
                    image_divide(imageInfo[0], divideBy, divideImageRT, imagePT, (BOOL *)divide5);
                    break;
                }
            }
            break;

        case 0x32: // 2 2번 사진 선택
            if (!isGameStart)
            {
                imageSelect = 1;
                switch (divideBy)
                {
                case 3:
                    image_divide(imageInfo[1], divideBy, divideImageRT, imagePT, (BOOL *)divide3);
                    break;
                case 4:
                    image_divide(imageInfo[1], divideBy, divideImageRT, imagePT, (BOOL *)divide4);
                    break;
                case 5:
                    image_divide(imageInfo[1], divideBy, divideImageRT, imagePT, (BOOL *)divide5);
                    break;
                }
            }
            break;

        case 0x33: // 3 3x3 그림 나누기
            if (!isGameStart)
            {
                divideBy = 3;
                image_divide(imageInfo[imageSelect], divideBy, divideImageRT, imagePT, (BOOL *)divide3);
            }
            break;

        case 0x34: // 4 4x4 그림 나누기
            if (!isGameStart)
            {
                divideBy = 4;
                image_divide(imageInfo[imageSelect], divideBy, divideImageRT, imagePT, (BOOL *)divide4);
            }
            break;

        case 0x35: // 5 5x5 그림 나누기
            if (!isGameStart)
            {
                divideBy = 5;
                image_divide(imageInfo[imageSelect], divideBy, divideImageRT, imagePT, (BOOL *)divide5);
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
        if (lbClick)
        {
            mx = LOWORD(lParam);
            my = HIWORD(lParam);
            lastPT = (POINT){mx, my};
            if (abs(firstPT.x - lastPT.x) > 100 || abs(firstPT.y - lastPT.y) > 100)
            {
                if (abs(firstPT.x - lastPT.x) > abs(firstPT.y - lastPT.y))
                {
                    if (firstPT.x > lastPT.x)
                    {
                        // 빈 공간 왼쪽에 있는 조각이 이동
                    }
                    else
                    {
                        // 빈 공간 오른쪽에 있는 조각이 이동
                    }
                }
                else
                {
                    if (firstPT.y > lastPT.y)
                    {
                        // 빈 공간 아래쪽에 있는 조각이 이동
                    }
                    else
                    {
                        // 빈 공간 위쪽에 있는 조각이 이동
                    }
                }
            }
        }
        break;

    case WM_LBUTTONUP:
        lbClick = FALSE;
        if (isMoving)
        {
            remainMove = 50;
            SetTimer(hWnd, TIME_TILE_MOVE, 50, NULL);
        }
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
                    StretchBlt(mdc, width * i, height * j, width, height,
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

        if (divideBy == 3)
            for (int i = 0; i < divideBy; ++i)
            {
                for (int j = 0; j < divideBy; ++j)
                {
                    if (divide3[j][i])
                        printf("1 ");
                    else
                        printf("0 ");
                }
                printf("\n");
            }
        if (divideBy == 4)
            for (int i = 0; i < divideBy; ++i)
            {
                for (int j = 0; j < divideBy; ++j)
                {
                    if (divide4[j][i])
                        printf("1 ");
                    else
                        printf("0 ");
                }
                printf("\n");
            }
        if (divideBy == 5)
            for (int i = 0; i < divideBy; ++i)
            {
                for (int j = 0; j < divideBy; ++j)
                {
                    if (divide5[j][i])
                        printf("1 ");
                    else
                        printf("0 ");
                }
                printf("\n");
            }

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

void image_divide(BITMAP imageInfo, int divideBy, RECT divideRT[][5], POINT *imagePT, BOOL *divide)
{
    double width = imageInfo.bmWidth / divideBy;
    double height = imageInfo.bmHeight / divideBy;

    int idx = 0;

    for (int i = 0; i < divideBy; ++i)
        for (int j = 0; j < divideBy; ++j)
        {
            // 각 이미지 좌표
            divideRT[i][j].left = width * i;
            divideRT[i][j].top = height * j;

            // 각 이미지 조각 크기
            divideRT[i][j].right = width;
            divideRT[i][j].bottom = height;

            imagePT[idx++] = (POINT){i, j};
        }

    shuffle(imagePT, divideBy * divideBy);

    idx = 0;
    for (int i = 0; i < divideBy; ++i)
        for (int j = 0; j < divideBy; ++j)
        {
            if (imagePT[idx].x == divideBy - 1 && imagePT[idx].y == divideBy - 1)
                *(divide + i * divideBy + j) = TRUE;
            else
                *(divide + i * divideBy + j) = FALSE;
            ++idx;
        }
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