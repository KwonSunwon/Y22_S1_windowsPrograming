#define _CRT_SECURE_NO_WARNINGS
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include <stdio.h>

#include <Windows.h>
#include <tchar.h>

typedef struct ImageType
{
    BOOL isFullsize;
    BOOL isInversion;
    BOOL isSplit4;
    BOOL isSplit9;
} ImageType;

typedef struct Selector
{
    RECT selectRT;
    int zoom;
    int move;
} Selector;

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
    HDC hdc, bitdc;
    PAINTSTRUCT ps;

    static HDC mdc;
    static HBITMAP backBit;

    static HBITMAP hImage;
    static BITMAP bit;
    static ImageType imageType;

    static Selector selector[3][3];
    static POINT select;
    static RECT selectRT;

    RECT drawRT;
    RECT winSizeRT;
    int bx, by;
    int mx, my;
    DWORD dword;

    switch (iMessage)
    {
    case WM_CREATE:
        // 이미지 로드
        hImage = (HBITMAP)LoadImage(NULL, L"image.bmp", IMAGE_BITMAP, 0, 0,
                                    LR_LOADFROMFILE | LR_CREATEDIBSECTION);
        // 이미지 정보
        GetObject(hImage, sizeof(BITMAP), &bit);

        // BOOL 초기화
        imageType.isFullsize = FALSE;
        imageType.isInversion = FALSE;
        imageType.isSplit4 = FALSE;
        imageType.isSplit9 = FALSE;

        // selector 초기화
        GetClientRect(hWnd, &winSizeRT);
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
            {
                selector[i][j].move = 200;
                selector[i][j].zoom = 0;
                selector[i][j].selectRT = winSizeRT;
            }

        select = (POINT){0, 0};

        break;

    case WM_LBUTTONDOWN:
        mx = LOWORD(lParam);
        my = HIWORD(lParam);

        GetClientRect(hWnd, &winSizeRT);

        if (imageType.isSplit4)
        {
            drawRT.right = winSizeRT.right / 2;
            drawRT.bottom = winSizeRT.bottom / 2;
            selectRT.left = (mx / (drawRT.right)) * drawRT.right;
            selectRT.top = (my / (drawRT.bottom)) * drawRT.bottom;
            selectRT.right = selectRT.left + drawRT.right;
            selectRT.bottom = selectRT.top + drawRT.bottom;
            selector[mx / (drawRT.right)][my / (drawRT.bottom)].selectRT = selectRT;
            select = (POINT){mx / (drawRT.right), my / (drawRT.bottom)};
        }
        else if (imageType.isSplit9)
        {
            drawRT.right = winSizeRT.right / 3;
            drawRT.bottom = winSizeRT.bottom / 3;
            selectRT.left = (mx / (drawRT.right)) * drawRT.right;
            selectRT.top = (my / (drawRT.bottom)) * drawRT.bottom;
            selectRT.right = selectRT.left + drawRT.right;
            selectRT.bottom = selectRT.top + drawRT.bottom;
            selector[mx / (drawRT.right)][my / (drawRT.bottom)].selectRT = selectRT;
            select = (POINT){mx / (drawRT.right), my / (drawRT.bottom)};
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        // A : 윈도우에 빈 공간 없이 배경 그림을 그림
        case 0x41:
            if (imageType.isFullsize)
                imageType.isFullsize = FALSE;
            else
                imageType.isFullsize = TRUE;
            break;

        // R : 색상을 반전/원본으로 출력
        case 0x52:
            if (imageType.isInversion)
                imageType.isInversion = FALSE;
            else
                imageType.isInversion = TRUE;
            break;

        // 2 : 4등분 해서 4칸에 각각 다른 레스터 연산을 하여 그림
        case 0x32:
            if (!imageType.isSplit4)
            {
                imageType.isSplit4 = TRUE;
                imageType.isSplit9 = FALSE;
            }
            else
                imageType.isSplit4 = FALSE;
            break;

        // 3 : 9등분 해서 9칸에 각각 다른 레스터 연산을 하여 그림
        case 0x33:
            if (!imageType.isSplit9)
            {
                imageType.isSplit9 = TRUE;
                imageType.isSplit4 = FALSE;
            }
            else
                imageType.isSplit9 = FALSE;
            break;

        // ←, → : 이미지가 좌/우로 조금씩 이동, 화면 밖으로 나가면 반대편에서 그려짐
        // 여러개의 그림이 있을 때는 마우스 왼쪽으로 선택한 그림만 이동(선택강조표시)
        case VK_LEFT:
            break;
        case VK_RIGHT:
            break;

        // +, - : 그림의 크기를 조금씩 확대/축소
        case VK_ADD:
            break;
        case VK_SUBTRACT:
            break;

        // Q : 프로그램 종료
        case 0x51:
            DeleteObject(hImage);
            PostQuitMessage(0);
            return 0;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_PAINT:
        GetClientRect(hWnd, &winSizeRT);
        drawRT = winSizeRT;

        hdc = BeginPaint(hWnd, &ps);
        mdc = CreateCompatibleDC(hdc);
        backBit = CreateCompatibleBitmap(hdc, winSizeRT.right, winSizeRT.bottom);
        SelectObject(mdc, (HBITMAP)backBit);
        PatBlt(mdc, 0, 0, winSizeRT.right, winSizeRT.bottom, WHITENESS);

        bitdc = CreateCompatibleDC(mdc);
        SelectObject(bitdc, (HBITMAP)hImage);

        bx = bit.bmWidth;
        by = bit.bmHeight;

        if (imageType.isSplit4)
        {
            drawRT.right /= 2;
            drawRT.bottom /= 2;

            for (int i = 0; i < 2; ++i)
                for (int j = 0; j < 2; ++j)
                {
                    if (i == j)
                        dword = SRCCOPY;
                    else
                        dword = NOTSRCCOPY;
                    if (select.x == i && select.y == j)
                    {
                        // StretchBlt(mdc, drawRT.right * i, drawRT.bottom * j, drawRT.right, drawRT.bottom, bitdc, 0, 0, bx - selector[i][j].zoom, by - selector[i][j].zoom, dword);
                        StretchBlt(mdc, drawRT.right * i, drawRT.bottom * j,
                                   drawRT.right - selector[i][j].move, drawRT.bottom,
                                   bitdc, selector[i][j].move, 0, bx - selector[i][j].move, by, dword);
                        StretchBlt(mdc, drawRT.right * i + (drawRT.right - selector[i][j].move), drawRT.bottom * j,
                                   selector[i][j].move, drawRT.bottom,
                                   bitdc, 0, 0, selector[i][j].move, by, dword);
                    }
                    else
                        StretchBlt(mdc, drawRT.right * i, drawRT.bottom * j, drawRT.right, drawRT.bottom, bitdc, 0, 0, bx, by, dword);
                }
        }
        else if (imageType.isSplit9)
        {
            drawRT.right /= 3;
            drawRT.bottom /= 3;

            dword = NOTSRCCOPY;

            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                {
                    if (dword == SRCCOPY)
                        dword = NOTSRCCOPY;
                    else
                        dword = SRCCOPY;
                    StretchBlt(mdc, drawRT.right * i, drawRT.bottom * j, drawRT.right, drawRT.bottom, bitdc, 0, 0, bx, by, dword);
                }
        }
        else
        {
            if (imageType.isInversion)
                dword = NOTSRCCOPY;
            else
                dword = SRCCOPY;

            if (imageType.isFullsize)
                StretchBlt(mdc, 0, 0, drawRT.right, drawRT.bottom, bitdc, 0, 0, bx, by, dword);
            else
                BitBlt(mdc, 0, 0, bx, by, bitdc, 0, 0, dword);
        }

        FrameRect(mdc, &selectRT, (HPEN)CreatePen(PS_SOLID, 5, RGB(255, 0, 0)));

        BitBlt(hdc, 0, 0, winSizeRT.right, winSizeRT.bottom, mdc, 0, 0, SRCCOPY);

        DeleteDC(bitdc);
        DeleteObject(backBit);
        DeleteDC(mdc);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        DeleteObject(hImage);
        PostQuitMessage(0);
        return 0;
    }
    return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}
