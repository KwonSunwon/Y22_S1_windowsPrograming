#define _CRT_SECURE_NO_WARNINGS
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include <stdio.h>

#include <Windows.h>
#include <tchar.h>

#include "resource.h"

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Window Programming Lab";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);

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
    HDC hdc;
    static HDC mdc, tmpDc;
    static HBITMAP backBit;
    static RECT bufferRT;

    switch (iMessage)
    {
    case WM_CREATE:
        break;

    case WM_LBUTTONDOWN:
        DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DialogProc);
        break;

    case WM_COMMAND:
        break;

    case BM_GETCHECK:
        printf("%d", wParam);
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &bufferRT);
        mdc = CreateCompatibleDC(hdc);
        backBit = CreateCompatibleBitmap(hdc, bufferRT.right, bufferRT.bottom);
        SelectObject(mdc, (HBITMAP)backBit);
        PatBlt(mdc, 0, 0, bufferRT.right, bufferRT.bottom, WHITENESS);

        // Draw, using mdc

        GetClientRect(hWnd, &bufferRT);
        BitBlt(hdc, 0, 0, bufferRT.right, bufferRT.bottom, mdc, 0, 0, SRCCOPY);
        DeleteObject(backBit);
        DeleteDC(mdc);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

BOOL CALLBACK DialogProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    HWND hRadio[4];
    static int shapeRadio;

    switch (iMsg)
    {
    case WM_INITDIALOG:
        CheckRadioButton(hDlg, IDC_SIN, IDC_DOUBLE, IDC_SIN);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            break;

        case IDCANCEL:
            EndDialog(hDlg, 0);
            break;

        case IDC_SIN:
            shapeRadio = 0;
            CheckRadioButton(hDlg, IDC_SIN, IDC_DOUBLE, IDC_SIN);
            SendMessage(hDlg, BM_GETCHECK, BST_CHECKED, 0);
            break;

        case IDC_ZIGZAG:
            shapeRadio = 1;
            CheckRadioButton(hDlg, IDC_SIN, IDC_DOUBLE, IDC_ZIGZAG);
            break;

        case IDC_SPRING:
            shapeRadio = 2;
            CheckRadioButton(hDlg, IDC_SIN, IDC_DOUBLE, IDC_SPRING);
            break;

        case IDC_DOUBLE:
            shapeRadio = 3;
            CheckRadioButton(hDlg, IDC_SIN, IDC_DOUBLE, IDC_DOUBLE);
            break;

        case IDC_RED:
            break;

        case IDC_GREEN:
            break;

        case IDC_BLUE:
            break;

        case IDC_INVERSION:
            break;

        case IDC_MOVEX:
            break;

        case IDC_MOVEY:
            break;

        case IDC_STOP:
            break;

        case IDC_RESET:
            break;

        case IDC_MOVECIRCLE:
            break;
        }
        break;

    case WM_CLOSE:
        EndDialog(hDlg, 0);
        break;
    }
    return 0;
}