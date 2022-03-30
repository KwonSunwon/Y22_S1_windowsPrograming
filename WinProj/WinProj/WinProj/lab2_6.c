#define _CRT_SECURE_NO_WARNINGS

//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
// debuging console

// 배경 색 채우기

#include <Windows.h>
#include <tchar.h>
#include <time.h>

#include <stdio.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Window Programming Lab";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

void drawX(int, TCHAR *);
void drawRevTriangle(int, TCHAR *);
void drawRhombus(int, TCHAR *);
void drawButterfly(int, TCHAR *);
void drawHourglass(int, TCHAR *);
void drawRectangle(int, TCHAR *);

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

    hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600,
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
    HDC hDC;

    static int row, col, x, y;
    static int colorCode[3];

    static RECT* rect;
    //static RECT rect[50];

    static TCHAR *str = L"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               ";

    // static TCHAR *strTest = L"This is passible?\r\nEnter";

    switch (iMessage)
    {
    case WM_CREATE:
        srand(time(NULL));
        row = rand() % 4 + 2;
        col = rand() % 4 + 3;
        // row = 5;
        // col = 5;

        x = 800 / col;
        y = 600 / row;

        rect = (RECT *)malloc(sizeof(RECT) * row * col); // -이유는 파악하지 못 했는데 동적할당을 하면 출력할 때 이상한 값이 들어감
        //  + 동적할당 해제할 위치를 잘 모르겠음(아마 윈도우가 죽을 때나 출력이 끝나고 나면 free해주면 될거 같은데 계속 오류 발생함)
        // 선언 개수 입력 받는 부분에서 sizeof(RECT) 를 안해줌...

        int idx = 0;
        for (int i = 0; i < col; ++i)
            for (int j = 0; j < row; ++j)
            {
                rect[idx].left = x * i;
                rect[idx].right = x * (i + 1) - 5;
                rect[idx].top = y * j;
                rect[idx].bottom = y * (j + 1);
                // printf("입력 rect[%d] (%d, %d, %d, %d)\n", idx, rect[idx].left, rect[idx].right, rect[idx].top, rect[idx].bottom);
                ++idx;
            }

        printf("col : %d, row : %d\nx : %d, y : %d\n", col, row, x, y);

        break;

    case WM_PAINT:
        hDC = BeginPaint(hWnd, &ps);

        for (int i = 0; i < row * col; ++i)
        {
            int randDraw = rand() % 6;
            // randDraw = 5;
            TCHAR drawStr[1000] = {'\0'};
            switch (randDraw)
            {
            case 0:
                drawX(x / 20, drawStr);
                break;
            case 1:
                drawRevTriangle(x / 20, drawStr);
                break;
            case 2:
                drawRhombus(x / 20, drawStr);
                break;
            case 3:
                drawButterfly(x / 20, drawStr);
                break;
            case 4:
                drawHourglass(x / 20, drawStr);
                break;
            case 5:
                drawRectangle(x / 20, drawStr);
                break;
            }
            for (int i = 0; i < 3; ++i)
                colorCode[i] = rand() % 255;
            SetBkColor(hDC, RGB(colorCode[2], colorCode[1], colorCode[0]));
            DrawText(hDC, str, lstrlen(str), &rect[i], DT_WORDBREAK | DT_EDITCONTROL);
            for (int i = 0; i < 3; ++i)
                colorCode[i] = rand() % 255;
            SetBkColor(hDC, RGB(colorCode[0], colorCode[1], colorCode[2]));
            SetTextColor(hDC, RGB(colorCode[0], colorCode[1], colorCode[2]));
            // SetTextColor(hDC, RGB(colorCode[0], colorCode[1], colorCode[2]));
            DrawText(hDC, drawStr, lstrlen(drawStr), &rect[i], DT_CENTER);
            printf("출력 rect[%d] (%d, %d, %d, %d)\n", i, rect[i].left, rect[i].right, rect[i].top, rect[i].bottom);
        }

        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        free(rect);
        HideCaret(hWnd);
        DestroyCaret();
        PostQuitMessage(0);
        break;
    }

    return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void drawX(int width, TCHAR *str)
{
    int blank = 0;
    int blank2 = width / 2;

    while (blank2 > 1)
    {
        for (int i = 0; i < blank; ++i)
            _tcscat(str, L" ");
        _tcscat(str, L"*");
        for (int i = 0; i < blank2; ++i)
            _tcscat(str, L" ");
        _tcscat(str, L"*\n");

        ++blank;
        blank2 -= 2;
    }

    while (blank2 <= width / 2)
    {
        for (int i = 0; i < blank; ++i)
            _tcscat(str, L" ");
        _tcscat(str, L"*");
        for (int i = 0; i < blank2; ++i)
            _tcscat(str, L" ");
        _tcscat(str, L"*\n");

        --blank;
        blank2 += 2;
    }
}
void drawRevTriangle(int width, TCHAR *str)
{
    int blank = 0;
    for (int i = 0; i < (width / 2); ++i)
    {
        for (int j = 0; j < blank; ++j)
        {
            _tcscat(str, L" ");
        }
        for (int k = 0; k < (width - blank * 2); ++k)
        {
            _tcscat(str, L"*");
        }
        _tcscat(str, L"\n");
        ++blank;
    }
}
void drawRhombus(int width, TCHAR *str)
{
    int blank = width / 2 - 1;
    int asterisk = 2;

    while (asterisk < width)
    {
        for (int i = 0; i < blank; ++i)
            _tcscat(str, L" ");
        for (int i = 0; i < asterisk; ++i)
            _tcscat(str, L"*");
        _tcscat(str, L"\n");
        blank -= 2;
        asterisk += 4;
    }

    while (asterisk >= 2)
    {
        for (int i = 0; i < blank; ++i)
            _tcscat(str, L" ");
        for (int i = 0; i < asterisk; ++i)
            _tcscat(str, L"*");
        _tcscat(str, L"\n");
        blank += 2;
        asterisk -= 4;
    }
}
void drawButterfly(int width, TCHAR *str)
{
    int blank = width - 2;
    int asterisk = 1;

    while (asterisk < width / 2)
    {
        for (int i = 0; i < asterisk; ++i)
            _tcscat(str, L"*");
        for (int i = 0; i < blank; ++i)
            _tcscat(str, L" ");
        for (int i = 0; i < asterisk; ++i)
            _tcscat(str, L"*");
        _tcscat(str, L"\n");
        blank -= 4;
        asterisk += 2;
    }

    if (asterisk != width / 2)
    {
        for (int i = 0; i < (asterisk - 1) * 2; ++i)
            _tcscat(str, L"*");
        _tcscat(str, L"\n");
        blank += 4;
        asterisk -= 2;
    }

    while (asterisk > 0)
    {
        for (int i = 0; i < asterisk; ++i)
            _tcscat(str, L"*");
        for (int i = 0; i < blank; ++i)
            _tcscat(str, L" ");
        for (int i = 0; i < asterisk; ++i)
            _tcscat(str, L"*");
        _tcscat(str, L"\n");
        blank += 4;
        asterisk -= 2;
    }
}
void drawHourglass(int width, TCHAR *str)
{
    int asterisk = width;
    int blank = 0;

    while (asterisk > 2)
    {
        for (int i = 0; i < blank; ++i)
        {
            _tcscat(str, L" ");
        }
        for (int i = 0; i < asterisk; ++i)
        {
            _tcscat(str, L"*");
        }
        _tcscat(str, L"\n");
        blank += 2;
        asterisk -= 4;
    }

    if (asterisk == 0)
    {
        for (int i = 0; i < width / 2 - 1; ++i)
            _tcscat(str, L" ");
        _tcscat(str, L"**\n");
        blank -= 2;
        asterisk += 4;
    }

    while (asterisk <= width)
    {
        for (int i = 0; i < blank; ++i)
        {
            _tcscat(str, L" ");
        }
        for (int i = 0; i < asterisk; ++i)
        {
            _tcscat(str, L"*");
        }
        _tcscat(str, L"\n");
        blank -= 2;
        asterisk += 4;
    }
}
void drawRectangle(int width, TCHAR *str)
{
    for (int i = 0; i < (width / 2); ++i)
    {
        if (i == 0 || i == ((width / 2) - 1))
            for (int j = 0; j < width; ++j)
                _tcscat(str, L"*");
        else
        {
            _tcscat(str, L"*");
            for (int j = 0; j < width - 2; ++j)
                _tcscat(str, L"*");
            _tcscat(str, L"*");
        }
        _tcscat(str, L"\n");
    }
}