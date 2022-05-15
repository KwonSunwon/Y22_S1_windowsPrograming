#define _CRT_SECURE_NO_WARNINGS
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include <stdio.h>

#include <Windows.h>
#include <tchar.h>

#define MAX_IMAGE 2

#define TIME_MAGINFIER_MOVE 1
#define LEFT_UP 0
#define RIGHT_UP 1
#define LEFT_DOWN 2
#define RIGHT_DOWN 3

typedef struct ImageSet
{
    HBITMAP image[MAX_IMAGE];
    BITMAP info[MAX_IMAGE];
    int select;
} ImageSet;

void imageSet_init(ImageSet *);

typedef struct Maginfier
{
    double width;
    double height;
    POINT location;
    BOOL active;
} Maginfier;

void maginfier_init(Maginfier *);
void maginfier_move(Maginfier *, int *, RECT);

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
    PAINTSTRUCT ps;
    HDC hdc, mdc;
    HBITMAP backBit;

    HDC bitDc;
    HDC bufferDc;
    RECT bufferRT;
    HBITMAP bufferBit, oldBitmap;

    static ImageSet image;

    HDC maginfierDc;
    HBITMAP maginfierBit;
    static Maginfier maginfier;
    static Maginfier duplicate;
    static int size;
    static int move;
    static int direction;

    static BOOL isCallChar; // Call WM_CHAR
    static BOOL isDuplicated;
    static BOOL isLClick;
    static BOOL isRClick;
    static BOOL isFullScreen;
    static BOOL isTopBottomInversion;
    static BOOL isLeftRightInversion;
    static BOOL isMoveMaginfier;

    static POINT first;
    static POINT last;
    static POINT movePT;

    double mx, my;
    double sourceWidth, sourceHeight;
    RECT tempRT;

    switch (iMessage)
    {
    case WM_CREATE:
        imageSet_init(&image);
        image.image[0] = (HBITMAP)LoadImage(NULL, L"image1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
        image.image[1] = (HBITMAP)LoadImage(NULL, L"image2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
        GetObject(image.image[0], sizeof(BITMAP), &image.info[0]);
        GetObject(image.image[1], sizeof(BITMAP), &image.info[1]);

        maginfier_init(&maginfier);
        maginfier_init(&duplicate);
        isCallChar = FALSE;
        isDuplicated = FALSE;
        isLClick = FALSE;
        isRClick = FALSE;
        isFullScreen = FALSE;
        isTopBottomInversion = FALSE;
        isLeftRightInversion = FALSE;
        size = 0;
        move = 0;
        direction = rand() % 4;

        break;

    case WM_TIMER:
        switch (wParam)
        {
        case TIME_MAGINFIER_MOVE:
            GetWindowRect(hWnd, &tempRT);
            maginfier_move(&maginfier, &direction, tempRT);
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        }
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case 0x31: // 1 첫 번째 이미지 선택
            image.select = 0;
            break;
        case 0x32: // 2 두 번째 이미지 선택
            image.select = 1;
            break;
        case 0x33: // 3 돋보기 내의 그림 확대
            size += 10;
            break;
        case 0x34: // 4 돋보기 내의 그림 축소
            size -= 10;
            break;
        case 0x30: // 0 돋보기 내의 그림 원래 크기로
            size = 0;
            break;
        case 0x43: // c 돋보기 내의 그림 복사
            isDuplicated = TRUE;
            duplicate.height = maginfier.height;
            duplicate.width = maginfier.width;
            break;
        case 0x50: // p 돋보기 내의 그림이 랜덤한 위치에 같은 크기로 복사
            if (isDuplicated)
            {
                duplicate.active = TRUE;
                GetWindowRect(hWnd, &tempRT);
                duplicate.location.x = rand() % 500 + 50;
                duplicate.location.y = rand() % 400 + 50;
            }
            break;
        case 0x46: // f 돋보기 내의 그림이 화면 전체에 복사, 다시 누르면 원래 크기로
            if (isFullScreen)
                isFullScreen = FALSE;
            else
                isFullScreen = TRUE;
            break;

        // 방향키 돋보기 위치가 이동, 붙여넣기한 곳의 그림은 움직인 돋보기 위치의 그림으로 그려짐
        case VK_LEFT:
            maginfier.location.x -= 10;
            break;
        case VK_RIGHT:
            maginfier.location.x += 10;
            break;
        case VK_UP:
            maginfier.location.y -= 10;
            break;
        case VK_DOWN:
            maginfier.location.y += 10;
            break;

        case 0x4D: // m 돋보기 사각형이 작아짐
            maginfier.width -= 10;
            maginfier.height -= 10;
            maginfier.location.x += 5;
            maginfier.location.y += 5;

            duplicate.height = maginfier.height;
            duplicate.width = maginfier.width;
            break;
        case 0x4E: // n 돋보기 사각형이 커짐
            maginfier.width += 10;
            maginfier.height += 10;
            maginfier.location.x -= 5;
            maginfier.location.y -= 5;

            duplicate.height = maginfier.height;
            duplicate.width = maginfier.width;
            break;
        case 0x48: // h 붙여넣기 한 곳의 그림이 좌우 반전
            if (isLeftRightInversion)
                isLeftRightInversion = FALSE;
            else
                isLeftRightInversion = TRUE;
            break;
        case 0x56: // v 붙여넣기 한 곳의 그림이 상하 반전
            if (isTopBottomInversion)
                isTopBottomInversion = FALSE;
            else
                isTopBottomInversion = TRUE;
            break;
        case 0x58: // X/x 붙여넣기 한 곳의 그림이 왼쪽/오른쪽으로 이동
            isCallChar = TRUE;
            break;
        case 0x59: // Y/y 사각형의 y축 크기 확대/축소
            isCallChar = TRUE;
            break;
        case 0x41: // a 돋보기 사각형 튕기기, 움직임에 따라 그림이 바뀜
            if (isMoveMaginfier)
            {
                isMoveMaginfier = FALSE;
                KillTimer(hWnd, TIME_MAGINFIER_MOVE);
            }
            else
            {
                isMoveMaginfier = TRUE;
                SetTimer(hWnd, TIME_MAGINFIER_MOVE, 30, NULL);
            }
            break;
        case 0x52: // r 리셋
            maginfier_init(&maginfier);
            maginfier_init(&duplicate);
            isCallChar = FALSE;
            isDuplicated = FALSE;
            isLClick = FALSE;
            isRClick = FALSE;
            isFullScreen = FALSE;
            isTopBottomInversion = FALSE;
            isLeftRightInversion = FALSE;
            size = 0;
            move = 0;
            direction = rand() % 4;
            KillTimer(hWnd, TIME_MAGINFIER_MOVE);
            break;
        case 0x51: // q 프로그램 종료
            DeleteObject(image.image[0]);
            DeleteObject(image.image[1]);
            PostQuitMessage(0);
            return 0;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_CHAR:
        if (isCallChar)
        {
            switch (wParam)
            {
            case 'X': // 붙여넣기 한 곳의 그림이 왼쪽으로 이동
                move -= 10;
                if (move < 0)
                    move = maginfier.width;
                break;
            case 'x': // 붙여넣기 한 곳의 그림이 오른쪽으로 이동
                move += 10;
                if (move > maginfier.width * 2)
                    move = maginfier.width;
                break;
            case 'Y': // 사각형의 y축 크기 확대
                maginfier.height += 10;
                duplicate.height = maginfier.height;
                break;
            case 'y': // 사각형의 y축 크기 축소
                maginfier.height -= 10;
                duplicate.height = maginfier.height;
                break;
            }
            isCallChar = FALSE;
            InvalidateRect(hWnd, NULL, FALSE);
        }
        break;

    case WM_LBUTTONDOWN:
        mx = LOWORD(lParam);
        my = HIWORD(lParam);
        first = (POINT){mx, my};
        last = first;
        isLClick = TRUE;
        break;

    case WM_RBUTTONDOWN:
        mx = LOWORD(lParam);
        my = HIWORD(lParam);
        first = (POINT){mx, my};

        tempRT.left = maginfier.location.x;
        tempRT.top = maginfier.location.y;
        tempRT.right = maginfier.location.x + maginfier.width;
        tempRT.bottom = maginfier.location.y + maginfier.height;
        if (PtInRect(&tempRT, first))
        {
            movePT.x = mx - tempRT.left;
            movePT.y = my - tempRT.top;
            isRClick = TRUE;
        }

        break;

    case WM_MOUSEMOVE:
        if (isLClick)
        {
            mx = LOWORD(lParam);
            my = HIWORD(lParam);

            hdc = GetDC(hWnd);

            SetROP2(hdc, R2_XORPEN);
            SelectObject(hdc, (HPEN)GetStockObject(WHITE_PEN));
            SelectObject(hdc, (HBRUSH)GetStockObject(BLACK_BRUSH));

            Rectangle(hdc, first.x, first.y, last.x, last.y);
            last = (POINT){mx, my};
            Rectangle(hdc, first.x, first.y, last.x, last.y);

            ReleaseDC(hWnd, hdc);
        }
        if (isRClick)
        {
            mx = LOWORD(lParam);
            my = HIWORD(lParam);

            maginfier.location.x = mx - movePT.x;
            maginfier.location.y = my - movePT.y;

            InvalidateRect(hWnd, NULL, FALSE);
        }
        break;

    case WM_LBUTTONUP:
        isLClick = FALSE;
        maginfier.active = TRUE;
        maginfier.location = (POINT){first.x, first.y};
        maginfier.width = last.x - first.x;
        maginfier.height = last.y - first.y;
        duplicate.height = maginfier.height;
        duplicate.width = maginfier.width;
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_RBUTTONUP:
        isRClick = FALSE;
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &bufferRT);
        mdc = CreateCompatibleDC(hdc);
        backBit = CreateCompatibleBitmap(hdc, bufferRT.right, bufferRT.bottom);
        SelectObject(mdc, (HBITMAP)backBit);
        PatBlt(mdc, 0, 0, bufferRT.right, bufferRT.bottom, WHITENESS);

        bitDc = CreateCompatibleDC(hdc);
        SelectObject(bitDc, (HBITMAP)image.image[image.select]);

        bufferDc = CreateCompatibleDC(hdc);
        bufferBit = CreateCompatibleBitmap(hdc, bufferRT.right, bufferRT.bottom);
        SelectObject(bufferDc, (HBITMAP)bufferBit);

        sourceWidth = image.info[image.select].bmWidth;
        sourceHeight = image.info[image.select].bmHeight;

        StretchBlt(bufferDc, 0, 0, bufferRT.right, bufferRT.bottom, bitDc, 0, 0, sourceWidth, sourceHeight, SRCCOPY);
        BitBlt(mdc, 0, 0, bufferRT.right, bufferRT.bottom, bufferDc, 0, 0, SRCCOPY);

        if (maginfier.active)
        {
            maginfierDc = CreateCompatibleDC(hdc);
            maginfierBit = CreateCompatibleBitmap(hdc, maginfier.width * 2, maginfier.height);
            SelectObject(maginfierDc, (HBITMAP)maginfierBit);

            StretchBlt(maginfierDc, 0, 0, maginfier.width, maginfier.height,
                       bufferDc, maginfier.location.x + size / 2, maginfier.location.y + size / 2,
                       maginfier.width - size, maginfier.height - size, SRCCOPY);
            StretchBlt(maginfierDc, maginfier.width, 0, maginfier.width, maginfier.height,
                       bufferDc, maginfier.location.x + size / 2, maginfier.location.y + size / 2,
                       maginfier.width - size, maginfier.height - size, SRCCOPY);

            if (!isFullScreen)
            {
                BitBlt(mdc, maginfier.location.x, maginfier.location.y, maginfier.width, maginfier.height,
                       maginfierDc, 0, 0, SRCCOPY);
                tempRT.left = maginfier.location.x;
                tempRT.top = maginfier.location.y;
                tempRT.right = maginfier.location.x + maginfier.width;
                tempRT.bottom = maginfier.location.y + maginfier.height;
                FrameRect(mdc, &tempRT, (HPEN)CreatePen(PS_SOLID, 3, RGB(200, 50, 50)));
            }
            else
                StretchBlt(mdc, 0, 0, bufferRT.right, bufferRT.bottom,
                           maginfierDc, 0, 0, maginfier.width, maginfier.height, SRCCOPY);

            if (duplicate.active)
            {
                if (!isTopBottomInversion && !isLeftRightInversion)
                    BitBlt(mdc, duplicate.location.x, duplicate.location.y, duplicate.width, duplicate.height,
                           maginfierDc, 0 + move, 0, SRCCOPY);
                else
                {
                    int nXD = duplicate.location.x;
                    int nYD = duplicate.location.y;
                    int nW = duplicate.width;
                    int nH = duplicate.height;
                    if (isLeftRightInversion)
                    {
                        nXD = duplicate.location.x + duplicate.width;
                        nW = -duplicate.width;
                    }
                    if (isTopBottomInversion)
                    {
                        nYD = duplicate.location.y + duplicate.height;
                        nH = -duplicate.height;
                    }
                    StretchBlt(mdc, nXD, nYD, nW, nH,
                               maginfierDc, 0 + move, 0, duplicate.width, duplicate.height, SRCCOPY);
                }

                tempRT.left = duplicate.location.x;
                tempRT.top = duplicate.location.y;
                tempRT.right = duplicate.location.x + duplicate.width;
                tempRT.bottom = duplicate.location.y + duplicate.height;
                FrameRect(mdc, &tempRT, (HPEN)CreatePen(PS_SOLID, 3, BLACK_PEN));
            }

            DeleteObject(maginfierBit);
            DeleteDC(maginfierDc);
        }

        DeleteObject(bufferBit);
        DeleteDC(bufferDc);
        DeleteDC(bitDc);

        GetClientRect(hWnd, &bufferRT);
        BitBlt(hdc, 0, 0, bufferRT.right, bufferRT.bottom, mdc, 0, 0, SRCCOPY);
        DeleteObject(backBit);
        DeleteDC(mdc);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        DeleteObject(image.image[0]);
        DeleteObject(image.image[1]);
        PostQuitMessage(0);
        return 0;
    }
    return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void imageSet_init(ImageSet *_imageSet)
{
    _imageSet->select = 0;
}

void maginfier_init(Maginfier *_maginfier)
{
    _maginfier->active = FALSE;
}

void maginfier_move(Maginfier *_maginfier, int *_direction, RECT _winRT)
{
    switch (*_direction)
    {
    case LEFT_UP:
        _maginfier->location.x -= 3;
        _maginfier->location.y -= 3;
        break;
    case RIGHT_UP:
        _maginfier->location.x += 3;
        _maginfier->location.y -= 3;
        break;
    case LEFT_DOWN:
        _maginfier->location.x -= 3;
        _maginfier->location.y += 3;
        break;
    case RIGHT_DOWN:
        _maginfier->location.x += 3;
        _maginfier->location.y += 3;
        break;
    }

    if (_maginfier->location.x <= 0)
    {
        if (*_direction == LEFT_UP)
            *_direction = RIGHT_UP;
        else
            *_direction = RIGHT_DOWN;
    }
    if (_maginfier->location.x >= _winRT.right - _maginfier->width)
    {
        printf("호출\n");
        if (*_direction == RIGHT_UP)
            *_direction = LEFT_UP;
        else if (*_direction == RIGHT_DOWN)
            *_direction = LEFT_DOWN;
        printf("%d\n", *_direction);
    }
    if (_maginfier->location.y <= 0)
    {
        if (*_direction == LEFT_UP)
            *_direction = LEFT_DOWN;
        else
            *_direction = RIGHT_DOWN;
    }
    if (_maginfier->location.y >= _winRT.bottom - _maginfier->height)
    {
        if (*_direction == LEFT_DOWN)
            *_direction = LEFT_UP;
        else
            *_direction = RIGHT_UP;
    }
}