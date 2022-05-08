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

void draw_bitmap(HDC, POINT, HDC, POINT, Selector, RECT, DWORD);

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
    static BITMAP bitInfo;
    static ImageType imageType;

    static Selector selector[3][3];
    static POINT select;
    static RECT selectRT;
    static BOOL isSelect;

    RECT drawRT;
    RECT winSizeRT;
    int bx, by;
    int mx, my;
    DWORD dword;

    switch (iMessage)
    {
    case WM_CREATE:
        // �̹��� �ε�
        hImage = (HBITMAP)LoadImage(NULL, L"image.bmp", IMAGE_BITMAP, 0, 0,
                                    LR_LOADFROMFILE | LR_CREATEDIBSECTION);
        // �̹��� ����
        GetObject(hImage, sizeof(BITMAP), &bitInfo);

        // BOOL �ʱ�ȭ
        imageType.isFullsize = FALSE;
        imageType.isInversion = FALSE;
        imageType.isSplit4 = FALSE;
        imageType.isSplit9 = FALSE;

        // selector �ʱ�ȭ
        GetClientRect(hWnd, &winSizeRT);
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
            {
                selector[i][j].move = 0;
                selector[i][j].zoom = 0;
                selector[i][j].selectRT = winSizeRT;
            }
        select = (POINT){0, 0};
        isSelect = FALSE;

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
            isSelect = TRUE;
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
            isSelect = TRUE;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_KEYDOWN:
        GetClientRect(hWnd, &winSizeRT);
        switch (wParam)
        {
        // A : �����쿡 �� ���� ���� ��� �׸��� �׸�
        case 0x41:
            if (imageType.isFullsize)
                imageType.isFullsize = FALSE;
            else
                imageType.isFullsize = TRUE;
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                {
                    selector[i][j].move = 0;
                    selector[i][j].zoom = 0;
                    selector[i][j].selectRT = winSizeRT;
                }
            break;

        // R : ������ ����/�������� ���
        case 0x52:
            if (imageType.isInversion)
                imageType.isInversion = FALSE;
            else
                imageType.isInversion = TRUE;
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                {
                    selector[i][j].move = 0;
                    selector[i][j].zoom = 0;
                    selector[i][j].selectRT = winSizeRT;
                }
            break;

        // 2 : 4��� �ؼ� 4ĭ�� ���� �ٸ� ������ ������ �Ͽ� �׸�
        case 0x32:
            if (!imageType.isSplit4)
            {
                imageType.isSplit4 = TRUE;
                imageType.isSplit9 = FALSE;

                isSelect = FALSE;

                for (int i = 0; i < 3; ++i)
                    for (int j = 0; j < 3; ++j)
                    {
                        selector[i][j].move = 0;
                        selector[i][j].zoom = 0;
                        selector[i][j].selectRT = winSizeRT;
                    }

                imageType.isFullsize = FALSE;
            }
            else
            {
                imageType.isSplit4 = FALSE;

                isSelect = FALSE;

                for (int i = 0; i < 3; ++i)
                    for (int j = 0; j < 3; ++j)
                    {
                        selector[i][j].move = 0;
                        selector[i][j].zoom = 0;
                        selector[i][j].selectRT = winSizeRT;
                    }
                select = (POINT){0, 0};
            }
            break;

        // 3 : 9��� �ؼ� 9ĭ�� ���� �ٸ� ������ ������ �Ͽ� �׸�
        case 0x33:
            if (!imageType.isSplit9)
            {
                imageType.isSplit9 = TRUE;
                imageType.isSplit4 = FALSE;

                isSelect = FALSE;

                for (int i = 0; i < 3; ++i)
                    for (int j = 0; j < 3; ++j)
                    {
                        selector[i][j].move = 0;
                        selector[i][j].zoom = 0;
                        selector[i][j].selectRT = winSizeRT;
                    }

                imageType.isFullsize = FALSE;
            }
            else
            {
                imageType.isSplit9 = FALSE;

                isSelect = FALSE;

                for (int i = 0; i < 3; ++i)
                    for (int j = 0; j < 3; ++j)
                    {
                        selector[i][j].move = 0;
                        selector[i][j].zoom = 0;
                        selector[i][j].selectRT = winSizeRT;
                    }
                select = (POINT){0, 0};
            }
            break;

        // ��, �� : �̹����� ��/��� ���ݾ� �̵�, ȭ�� ������ ������ �ݴ����� �׷���
        // �������� �׸��� ���� ���� ���콺 �������� ������ �׸��� �̵�(���ð���ǥ��)
        case VK_LEFT:
            if (imageType.isSplit4 || imageType.isSplit9)
            {
                if (isSelect)
                    selector[select.x][select.y].move += 10;
            }
            else
                selector[select.x][select.y].move += 10;
            break;

        case VK_RIGHT:
            if (imageType.isSplit4 || imageType.isSplit9)
            {
                if (isSelect)
                    selector[select.x][select.y].move -= 10;
            }
            else
                selector[select.x][select.y].move -= 10;
            break;

        // +, - : �׸��� ũ�⸦ ���ݾ� Ȯ��/���
        case VK_ADD:
            if (imageType.isSplit4 || imageType.isSplit9)
            {
                if (isSelect)
                    selector[select.x][select.y].zoom += 10;
            }
            else
                selector[select.x][select.y].zoom += 10;
            break;

        case VK_SUBTRACT:
            if (imageType.isSplit4 || imageType.isSplit9)
            {
                if (isSelect)
                    selector[select.x][select.y].zoom -= 10;
            }
            else
                selector[select.x][select.y].zoom -= 10;
            break;

        // Q : ���α׷� ����
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

        // �������� �����ŭ ���ο� ��Ʈ��(backBit)�� ������
        backBit = CreateCompatibleBitmap(hdc, winSizeRT.right, winSizeRT.bottom);

        // ��Ʈ��(backBit) �����ؼ� bufferDC(memDc)�� ����� ������� ĥ����
        SelectObject(mdc, (HBITMAP)backBit);
        PatBlt(mdc, 0, 0, winSizeRT.right, winSizeRT.bottom, WHITENESS);

        // WM_CREATE���� �ҷ��� ��Ʈ�� �̹���(hImage)�� ��������
        bitdc = CreateCompatibleDC(mdc);
        SelectObject(bitdc, (HBITMAP)hImage);

        bx = bitInfo.bmWidth;
        by = bitInfo.bmHeight;

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

                    if (selector[i][j].move < 0)
                        selector[i][j].move = drawRT.right;

                    draw_bitmap(mdc, (POINT){i, j}, bitdc, (POINT){bx, by}, selector[i][j], drawRT, dword);
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

                    if (selector[i][j].move < 0)
                        selector[i][j].move = drawRT.right;

                    draw_bitmap(mdc, (POINT){i, j}, bitdc, (POINT){bx, by}, selector[i][j], drawRT, dword);
                }
        }
        else
        {
            if (selector[0][0].move < 0)
                selector[0][0].move = drawRT.right;

            if (imageType.isInversion)
                dword = NOTSRCCOPY;
            else
                dword = SRCCOPY;

            if (imageType.isFullsize)
                draw_bitmap(mdc, (POINT){0, 0}, bitdc, (POINT){bx, by}, selector[0][0], drawRT, dword);
            else
            {
                HDC bufferDc = CreateCompatibleDC(hdc);
                HBITMAP bufferBitmap = CreateCompatibleBitmap(hdc, drawRT.right * 2, drawRT.bottom);
                SelectObject(bufferDc, (HBITMAP)bufferBitmap);
                PatBlt(bufferDc, 0, 0, drawRT.right * 2, drawRT.bottom, WHITENESS);

                BitBlt(bufferDc, 0, 0, drawRT.right, drawRT.bottom, bitdc, 0, 0, dword);
                BitBlt(bufferDc, drawRT.right, 0, drawRT.right, drawRT.bottom, bitdc, 0, 0, dword);

                StretchBlt(bufferDc, 0, 0, bx + selector[0][0].zoom, by + selector[0][0].zoom, bitdc, 0, 0, bx, by, dword);
                StretchBlt(bufferDc, drawRT.right, 0, bx + selector[0][0].zoom, by + selector[0][0].zoom, bitdc, 0, 0, bx, by, dword);

                BitBlt(mdc, 0, 0, drawRT.right, drawRT.bottom, bufferDc, selector[0][0].move % drawRT.right, 0, SRCCOPY);

                DeleteObject(bufferBitmap);
                DeleteDC(bufferDc);
            }
        }

        if (isSelect)
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

void draw_bitmap(HDC destDc, POINT index, HDC bitmapDc, POINT bitmapInfo, Selector movementInfo, RECT size, DWORD dword)
{
    HDC bufferDc = CreateCompatibleDC(destDc);
    HBITMAP bufferBitmap = CreateCompatibleBitmap(destDc, size.right * 2, size.bottom);
    SelectObject(bufferDc, (HBITMAP)bufferBitmap);
    PatBlt(bufferDc, 0, 0, size.right * 2, size.bottom, WHITENESS);

    // Ȯ��� ũ��� �� ���� ��Ʈ���� ���޾Ƽ� �׷���
    StretchBlt(bufferDc, 0, 0, size.right, size.bottom,
               bitmapDc, 0 + movementInfo.zoom / 2, 0 + movementInfo.zoom / 2, bitmapInfo.x - movementInfo.zoom, bitmapInfo.y - movementInfo.zoom, dword);
    StretchBlt(bufferDc, size.right, 0, size.right, size.bottom,
               bitmapDc, 0 + movementInfo.zoom / 2, 0 + movementInfo.zoom / 2, bitmapInfo.x - movementInfo.zoom, bitmapInfo.y - movementInfo.zoom, dword);

    // �� ���� ��Ʈ���� �׸� DC���� move ��ŭ �̵��ؼ� ȭ���� �׷��� (move�� �׷��� �� �ִ� ũ�⸸ŭ(size.right)�� ������ ������ ������ �̵�)
    BitBlt(destDc, size.right * index.x, size.bottom * index.y, size.right, size.bottom, bufferDc, movementInfo.move % size.right, 0, SRCCOPY);

    DeleteObject(bufferBitmap);
    DeleteDC(bufferDc);
}