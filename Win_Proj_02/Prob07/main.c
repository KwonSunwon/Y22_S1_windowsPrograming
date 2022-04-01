#include <Windows.h>
#include <tchar.h>
#include <time.h>

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

    hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 800, 650,
                        NULL, (HMENU)NULL, hInstance, NULL);
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

    HPEN hPen, oldPen;
    HBRUSH hBrush, oldBrush;

    static int centerShape;
    static COLORREF color[4];
    static COLORREF oldColor;

    static int keyDown;
    static int pressArrow;

    RECT center = {300, 200, 500, 400};

    POINT left[] = {{200, 220}, {120, 380}, {280, 380}};
    POINT up[] = {{320, 20}, {480, 20}, {400, 100}, {320, 180}, {480, 180}};
    POINT right[] = {{600, 220}, {520, 280}, {550, 380}, {650, 380}, {680, 280}};
    int pie[] = {320, 420, 480, 580, 400, 420, 480, 500};

    POINT centerLeft[] = {{400, 220}, {320, 380}, {480, 380}};
    POINT centerUp[] = {{320, 220}, {480, 220}, {400, 300}, {320, 380}, {480, 380}};
    POINT centerRight[] = {{400, 220}, {320, 280}, {350, 380}, {450, 380}, {480, 280}};
    int centerPie[] = {320, 220, 480, 380, 400, 220, 480, 300};

    POINT miniLeft[] = {{400, 250}, {350, 350}, {450, 350}};
    POINT miniUp[] = {{350, 250}, {450, 250}, {400, 300}, {350, 350}, {450, 350}};
    POINT miniRight[] = {{400, 250}, {350, 288}, {370, 350}, {430, 350}, {450, 288}};
    int miniPie[] = {350, 250, 450, 350, 400, 250, 450, 300};

    switch (iMessage)
    {
    case WM_CREATE:
        srand(time(NULL));
        // centerShape = rand() % 4;
        centerShape = 3;
        for (int i = 0; i < 4; ++i)
            color[i] = RGB(rand() % 255, rand() % 255, rand() % 255);
        keyDown = 0;
        pressArrow = 0;
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case 0x54: // t
            centerShape = 0;
            if (!keyDown)
            {
                keyDown = 1;
                oldColor = color[0];
                color[0] = RGB(rand() % 255, rand() % 255, rand() % 255);
            }
            pressArrow = 0;
            break;
        case 0x53: // s
            centerShape = 1;
            if (!keyDown)
            {
                keyDown = 1;
                oldColor = color[1];
                color[1] = RGB(rand() % 255, rand() % 255, rand() % 255);
            }
            pressArrow = 0;
            break;
        case 0x50: // p
            centerShape = 2;
            if (!keyDown)
            {
                keyDown = 1;
                oldColor = color[2];
                color[2] = RGB(rand() % 255, rand() % 255, rand() % 255);
            }
            pressArrow = 0;
            break;
        case 0x45: // e
            centerShape = 3;
            if (!keyDown)
            {
                keyDown = 1;
                oldColor = color[3];
                color[3] = RGB(rand() % 255, rand() % 255, rand() % 255);
            }
            pressArrow = 0;
            break;
        case VK_LEFT:
            centerShape = 0;
            pressArrow = 1;
            break;
        case VK_UP:
            centerShape = 1;
            pressArrow = 1;
            break;
        case VK_RIGHT:
            centerShape = 2;
            pressArrow = 1;
            break;
        case VK_DOWN:
            centerShape = 3;
            pressArrow = 1;
            break;
        case 0x51: // q
            PostQuitMessage(0);
            return 0;
        }
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_KEYUP:
        switch (wParam)
        {
        case 0x54: // t
            keyDown = 0;
            color[0] = oldColor;
            break;
        case 0x53: // s
            keyDown = 0;
            color[1] = oldColor;
            break;
        case 0x50: // p
            keyDown = 0;
            color[2] = oldColor;
            break;
        case 0x45: // e
            keyDown = 0;
            color[3] = oldColor;
            break;
        }
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

        // 가운데 사각형
        hPen = CreatePen(PS_SOLID, 1, BLACK_BRUSH);
        oldPen = (HPEN)SelectObject(hdc, hPen);
        FrameRect(hdc, &center, hPen);
        SelectObject(hdc, oldPen);

        // 왼쪽 삼각형
        hBrush = CreateSolidBrush(color[0]);
        oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
        Polygon(hdc, left, 3);
        SelectObject(hdc, oldBrush);

        // 위쪽 모래시계
        hBrush = CreateSolidBrush(color[1]);
        oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
        Polygon(hdc, up, 5);
        SelectObject(hdc, oldBrush);

        // 오른쪽 오각형
        hBrush = CreateSolidBrush(color[2]);
        oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
        Polygon(hdc, right, 5);
        SelectObject(hdc, oldBrush);

        // 아래쪽 파이
        hBrush = CreateSolidBrush(color[3]);
        oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
        Pie(hdc, pie[0], pie[1], pie[2], pie[3], pie[4], pie[5], pie[6], pie[7]);
        SelectObject(hdc, oldBrush);

        // 가운데 임의의 도형
        switch (centerShape)
        {
        case 0: // 삼각형
            hBrush = CreateSolidBrush(color[centerShape]);
            oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
            if (pressArrow)
                Polygon(hdc, miniLeft, 3);
            else
                Polygon(hdc, centerLeft, 3);
            SelectObject(hdc, oldBrush);
            break;

        case 1: // 모래시계
            hBrush = CreateSolidBrush(color[centerShape]);
            oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
            if (pressArrow)
                Polygon(hdc, miniUp, 5);
            else
                Polygon(hdc, centerUp, 5);
            SelectObject(hdc, oldBrush);
            break;

        case 2: // 오각형
            hBrush = CreateSolidBrush(color[centerShape]);
            oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
            if (pressArrow)
                Polygon(hdc, miniRight, 5);
            else
                Polygon(hdc, centerRight, 5);
            SelectObject(hdc, oldBrush);
            break;

        case 3: // 파이
            hBrush = CreateSolidBrush(color[3]);
            oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
            if (pressArrow)
                Pie(hdc, miniPie[0], miniPie[1], miniPie[2], miniPie[3], miniPie[4], miniPie[5], miniPie[6], miniPie[7]);
            else
                Pie(hdc, centerPie[0], centerPie[1], centerPie[2], centerPie[3], centerPie[4], centerPie[5], centerPie[6], centerPie[7]);
            SelectObject(hdc, oldBrush);
            break;
        }

        DeleteObject(hPen);
        DeleteObject(hBrush);

        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}