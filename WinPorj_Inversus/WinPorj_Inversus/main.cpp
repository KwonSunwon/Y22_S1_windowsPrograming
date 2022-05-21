#define _CRT_SECURE_NO_WARNINGS
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#include "stdafx.h"

#include "enemy.h"
#include "player.h"
#include "Map.h"

POINT position_to_point(POINT);
BOOL intersect_check(Player, Map, int);

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Inversus";
LPCTSTR lpszWindowName = L"Inversus";

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
    RECT bufferRT;

    static Map map;
    static Player player;

    static int level;

    RECT tempRT;
    POINT tempPT;
    int tempObj;
    RECT tempMapRT;
    RECT tempPlayerRT;

    switch (iMessage)
    {
    case WM_CREATE:
        level = map.get_level();
        player.init(level);
        SetTimer(hWnd, BULLET_RENDER, 40, NULL);
        break;

    case WM_TIMER:
        switch (wParam)
        {
        case BULLET_RENDER:
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        }
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case 0x41: // a
            if (!intersect_check(player, map, LEFT))
                player.move(LEFT, map.get_map_size());
            break;
        case 0x44: // d
            if (!intersect_check(player, map, RIGHT))
                player.move(RIGHT, map.get_map_size());
            break;
        case 0x57: // w
            if (!intersect_check(player, map, UP))
                player.move(UP, map.get_map_size());
            break;
        case 0x53: // s
            if (!intersect_check(player, map, DOWN))
                player.move(DOWN, map.get_map_size());
            break;

        case VK_LEFT:
            break;
        case VK_RIGHT:
            break;
        case VK_UP:
            break;
        case VK_DOWN:
            break;
        }

        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &bufferRT);
        mdc = CreateCompatibleDC(hdc);
        backBit = CreateCompatibleBitmap(hdc, bufferRT.right, bufferRT.bottom);
        SelectObject(mdc, (HBITMAP)backBit);
        PatBlt(mdc, 0, 0, bufferRT.right, bufferRT.bottom, WHITENESS);

        map.draw(mdc);
        player.draw(mdc);

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

POINT position_to_point(POINT _position)
{
    POINT temp = {
        (_position.x - MAP_LOCATION) / MAP_SIZE,
        (_position.y - MAP_LOCATION) / MAP_SIZE,
    };
    std::cout << temp.x << ", " << temp.y << "\n";
    return temp;
}

BOOL intersect_check(Player player, Map map, int direction)
{
    BOOL isIntersect = FALSE;

    POINT playerPos = player.get_position();
    POINT playerPt;
    RECT playerRt = player.get_pos_rect();
    RECT mapRt;
    RECT rcTemp;

    switch (direction)
    {
    case LEFT:
        playerPos.x -= (PLAYER_SIZE + 10);
        playerPt = position_to_point(playerPos);
        playerPt.y -= 1;
        for (int i = 0; i < 3; ++i)
        {
            mapRt = map.get_tile_rect(playerPt);
            if (IntersectRect(&rcTemp, &playerRt, &mapRt) && map.get_object(playerPt) != 0)
                isIntersect = TRUE;
            playerPt.y++;
        }
        break;
    case RIGHT:
        playerPos.x += (PLAYER_SIZE + 10);
        playerPt = position_to_point(playerPos);
        playerPt.y -= 1;
        for (int i = 0; i < 3; ++i)
        {
            mapRt = map.get_tile_rect(playerPt);
            if (IntersectRect(&rcTemp, &playerRt, &mapRt) && map.get_object(playerPt) != 0)
                isIntersect = TRUE;
            playerPt.y++;
        }
        break;
    case UP:
        playerPos.y -= (PLAYER_SIZE + 10);
        playerPt = position_to_point(playerPos);
        playerPt.x -= 1;
        for (int i = 0; i < 3; ++i)
        {
            mapRt = map.get_tile_rect(playerPt);
            if (IntersectRect(&rcTemp, &playerRt, &mapRt) && map.get_object(playerPt) != 0)
                isIntersect = TRUE;
            playerPt.x++;
        }
        break;
    case DOWN:
        playerPos.y += (PLAYER_SIZE + 10);
        playerPt = position_to_point(playerPos);
        playerPt.x -= 1;
        for (int i = 0; i < 3; ++i)
        {
            mapRt = map.get_tile_rect(playerPt);
            if (IntersectRect(&rcTemp, &playerRt, &mapRt) && map.get_object(playerPt) != 0)
                isIntersect = TRUE;
            playerPt.x++;
        }
        break;
    }

    return isIntersect;
}