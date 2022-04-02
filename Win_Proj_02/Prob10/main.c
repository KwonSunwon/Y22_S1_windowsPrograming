#define _CRT_SECURE_NO_WARNINGS
// #pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include <stdio.h>

#include <Windows.h>
#include <tchar.h>
#include <time.h>

typedef struct _SHAPE
{
    int shapeType;
    RECT location;
    int lineThick;
    COLORREF lineColor;
    COLORREF innerColor;
} SHAPE;

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
    HDC hdc;

    HPEN hPen, oldPen;
    HBRUSH hBrush, oldBrush;

    static SIZE size;

    static SHAPE shapeArr[10];
    static int shapeIdx;
    static int shapeCnt;

    static int error;
    TCHAR isFullText[] = L"Storage is full.";
    TCHAR errorText[] = L"There is no figure.";
    TCHAR textClear[] = L"                       ";

    RECT inputBox = {10, 530, 410, 550};
    static TCHAR inputStr[50];
    static int inputCnt;

    switch (iMessage)
    {
    case WM_CREATE:
        CreateCaret(hWnd, NULL, 5, 15);
        ShowCaret(hWnd);
        error = 0;
        shapeIdx = -1;
        shapeCnt = 0;
        inputCnt = 0;
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ADD:
            if (shapeCnt != 0)
            {
                if (shapeArr[shapeIdx].lineThick >= 10)
                    InflateRect(&shapeArr[shapeIdx].location, 5, 5);
                else
                    ++shapeArr[shapeIdx].lineThick;
            }
            else
                error = 2;
            break;
        case VK_SUBTRACT:
            if (shapeCnt != 0)
            {
                if (shapeArr[shapeIdx].lineThick <= 1)
                    InflateRect(&shapeArr[shapeIdx].location, -5, -5);
                else
                    --shapeArr[shapeIdx].lineThick;
            }
            else
                error = 2;
            break;

        case VK_UP:
            if (shapeCnt != 0)
                OffsetRect(&shapeArr[shapeIdx].location, 0, -5);
            else
                error = 2;
            break;
        case VK_LEFT:
            if (shapeCnt != 0)
                OffsetRect(&shapeArr[shapeIdx].location, -5, 0);
            else
                error = 2;
            break;
        case VK_DOWN:
            if (shapeCnt != 0)
                OffsetRect(&shapeArr[shapeIdx].location, 0, 5);
            else
                error = 2;
            break;
        case VK_RIGHT:
            if (shapeCnt != 0)
                OffsetRect(&shapeArr[shapeIdx].location, 5, 0);
            else
                error = 2;
            break;

        case 0x4B: // k - 내부 또는 테두리 색상 변경
            if (shapeCnt != 0)
            {
                shapeArr[shapeIdx].innerColor = RGB(rand() % 255, rand() % 255, rand() % 255);
                shapeArr[shapeIdx].lineColor = RGB(rand() % 255, rand() % 255, rand() % 255);
            }
            else
                error = 2;
            break;

        case 0x50: // p - 바로 전의 도형 다시 그리기
            if (shapeCnt != 0)
            {
                if (shapeIdx != 0)
                    --shapeIdx;
                else if (shapeIdx == 0)
                    shapeIdx = shapeCnt - 1;
            }
            else
                error = 2;
            break;

        case 0x4E: // n - 다음 도형 다시 그리기
            if (shapeCnt != 0)
            {
                if (shapeCnt - 1 == shapeIdx)
                    shapeIdx = 0;
                else
                    ++shapeIdx;
            }
            else
                error = 2;
            break;
        }

        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_CHAR:
        switch (wParam)
        {
        case VK_RETURN:
            int strIdx = 0;
            int temp[6] = {0};
            int tempIdx = 0;
            while (inputStr[strIdx] != 0x00)
            {
                temp[tempIdx++] = _ttoi(inputStr + strIdx);

                ++strIdx;
                while (inputStr[strIdx] != 32)
                {
                    ++strIdx;
                    if (inputStr[strIdx] == 0x00)
                        break;
                }

                printf("%d : %d\n", tempIdx, temp[tempIdx - 1]);
            }
            inputCnt = 0;
            inputStr[inputCnt] = 0x00;

            if (error == 0)
                if (temp[0] < 5 && temp[0] > 0)
                {
                    shapeArr[shapeCnt].shapeType = temp[0];
                    RECT rcTemp = {temp[1], temp[2], temp[3], temp[4]};
                    shapeArr[shapeCnt].location = rcTemp;
                    shapeArr[shapeCnt].lineThick = temp[5];
                    shapeArr[shapeCnt].lineColor = RGB(rand() % 255, rand() % 255, rand() % 255);
                    shapeArr[shapeCnt].innerColor = RGB(rand() % 255, rand() % 255, rand() % 255);
                    shapeIdx = shapeCnt++;
                    if (shapeCnt == 10)
                        error = 1;
                }
            break;

        case VK_BACK:
            if (inputCnt != 0)
                inputStr[--inputCnt] = 0x00;
            break;

        default:
            if (wParam >= '0' && wParam <= '9' || wParam == ' ')
            {
                inputStr[inputCnt++] = wParam;
                inputStr[inputCnt] = 0x00;
            }
            break;
        }
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

        // 도형 그리기
        hPen = CreatePen(PS_SOLID, shapeArr[shapeIdx].lineThick, shapeArr[shapeIdx].lineColor);
        oldPen = (HPEN)SelectObject(hdc, hPen);
        hBrush = CreateSolidBrush(shapeArr[shapeIdx].innerColor);
        oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
        switch (shapeArr[shapeIdx].shapeType)
        {

        case 1: // 선
            MoveToEx(hdc, shapeArr[shapeIdx].location.left, shapeArr[shapeIdx].location.top, NULL);
            LineTo(hdc, shapeArr[shapeIdx].location.right, shapeArr[shapeIdx].location.bottom);
            break;

        case 2: // 삼각형
            POINT pointTemp[3] = {
                {(shapeArr[shapeIdx].location.left + shapeArr[shapeIdx].location.right) / 2, shapeArr[shapeIdx].location.top},
                {shapeArr[shapeIdx].location.left, shapeArr[shapeIdx].location.bottom},
                {shapeArr[shapeIdx].location.right, shapeArr[shapeIdx].location.bottom},
            };
            Polygon(hdc, pointTemp, 3);
            break;

        case 3: // 사각형
            Rectangle(hdc, shapeArr[shapeIdx].location.left, shapeArr[shapeIdx].location.top,
                      shapeArr[shapeIdx].location.right, shapeArr[shapeIdx].location.bottom);
            break;

        case 4: // 원
            Ellipse(hdc, shapeArr[shapeIdx].location.left, shapeArr[shapeIdx].location.top,
                    shapeArr[shapeIdx].location.right, shapeArr[shapeIdx].location.bottom);
            break;
        }
        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);

        // 명령어 입력창 및 명령어 출력
        switch (error)
        {
        case 1:
            DrawText(hdc, isFullText, lstrlen(isFullText), &inputBox, NULL);
            GetTextExtentPoint32(hdc, isFullText, lstrlen(isFullText), &size);
            break;
        case 2:
            DrawText(hdc, errorText, lstrlen(errorText), &inputBox, NULL);
            error = 0;
            GetTextExtentPoint32(hdc, errorText, lstrlen(errorText), &size);
            break;
        default:
            DrawText(hdc, inputStr, lstrlen(inputStr), &inputBox, NULL);
            GetTextExtentPoint32(hdc, inputStr, lstrlen(inputStr), &size);
            break;
        }
        FrameRect(hdc, &inputBox, CreateSolidBrush(BLACK_BRUSH));

        SetCaretPos(size.cx + inputBox.left, inputBox.top + 2);

        DeleteObject(hPen);
        DeleteObject(hBrush);

        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        HideCaret(hWnd);
        DestroyCaret();
        PostQuitMessage(0);
        return 0;
    }
    return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}