#define _CRT_SECURE_NO_WARNINGS
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#include "stdafx.h"

#include "enemy.h"
#include "player.h"
#include "Map.h"
#include "bullet.h"

POINT position_to_point(POINT);

void bullet_shoot(Player *, Bullet[], int, int);
BOOL intersect_player_to_map(Player, Map, int);
BOOL intersect_bullet_to_map(Bullet *, Map *);
BOOL intersect_bullet_to_enemy(Bullet *, Enemy *);
BOOL intersect_player_to_enemy(Player *, Enemy *);

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
    static HDC hdc, mdc, timeDc;
    HBITMAP backBit;
    RECT bufferRT;

    static BOOL isGameStart;

    static Map map;
    static Player player;

    static int level;

    static Bullet bullet[12];
    static int bulletIdx;

    BulletInfo info;

    RECT tempRT;
    POINT tempPT;
    int tempObj;
    RECT tempMapRT;
    RECT tempPlayerRT;

    switch (iMessage)
    {
    case WM_CREATE:
        isGameStart = FALSE;
        bulletIdx = 0;
        level = map.get_level();
        break;

    case WM_TIMER:
        switch (wParam)
        {
        case BULLET_RENDER:
            player.in_bullet_rotate();
            break;
        case BULLET_MOVE:
            for (int i = 0; i < MAX_BULLET_DRAW; ++i)
            {
                if (bullet[i].get_is_live())
                {
                    bullet[i].move();
                    intersect_bullet_to_map(&bullet[i], &map);
                }
            }
            break;
        case BULLET_RELOAD:
            player.bullet_reload();
            break;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_KEYDOWN:
        if (isGameStart)
            switch (wParam)
            {
            case 0x41: // a
                if (!intersect_player_to_map(player, map, LEFT))
                    player.move(LEFT, map.get_map_size());
                if (intersect_player_to_map(player, map, LEFT))
                    player.move(RIGHT, map.get_map_size());
                break;
            case 0x44: // d
                if (!intersect_player_to_map(player, map, RIGHT))
                    player.move(RIGHT, map.get_map_size());
                if (intersect_player_to_map(player, map, RIGHT))
                    player.move(LEFT, map.get_map_size());
                break;
            case 0x57: // w
                if (!intersect_player_to_map(player, map, UP))
                    player.move(UP, map.get_map_size());
                if (intersect_player_to_map(player, map, UP))
                    player.move(DOWN, map.get_map_size());
                break;
            case 0x53: // s
                if (!intersect_player_to_map(player, map, DOWN))
                    player.move(DOWN, map.get_map_size());
                if (intersect_player_to_map(player, map, DOWN))
                    player.move(UP, map.get_map_size());
                break;

            case VK_LEFT:
                bullet_shoot(&player, bullet, bulletIdx, LEFT);
                break;
            case VK_RIGHT:
                bullet_shoot(&player, bullet, bulletIdx, RIGHT);
                break;
            case VK_UP:
                bullet_shoot(&player, bullet, bulletIdx, UP);
                break;
            case VK_DOWN:
                bullet_shoot(&player, bullet, bulletIdx, DOWN);
                break;
            }
        else
            switch (wParam)
            {
            case 0x31: // 1 난이도 변경
                map.change(EASY);
                break;
            case 0x32: // 2 난이도 변경
                map.change(NORMAL);
                break;
            case 0x33: // 3 난이도 변경
                map.change(HARD);
                break;

            case VK_RETURN: // 게임 시작
                isGameStart = TRUE;
                player.init(map.get_level());
                SetTimer(hWnd, BULLET_RENDER, 40, NULL);
                SetTimer(hWnd, BULLET_MOVE, 10, NULL);
                SetTimer(hWnd, BULLET_RELOAD, BULLET_RELOAD_TIME, NULL);
                break;
            }
        if (bulletIdx++ == MAX_BULLET_DRAW + 1)
            bulletIdx = 0;
        InvalidateRect(hWnd, NULL, FALSE);
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
        for (int i = 0; i < 12; ++i)
            if (bullet[i].get_is_live())
                bullet[i].draw(mdc);

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
    return temp;
}

BOOL intersect_player_to_map(Player player, Map map, int direction)
{
    BOOL isIntersect = FALSE;

    POINT playerPos = player.get_position();
    POINT playerPt;
    RECT playerRt = player.get_pos_rect();
    RECT mapRt;
    RECT rcTemp;

    RECT mapSize = map.get_map_size();

    switch (direction)
    {
    case LEFT:
        playerPos.x -= PLAYER_SIZE;
        playerPt = position_to_point(playerPos);
        playerPt.y -= 1;
        for (int i = 0; i < 3; ++i)
        {
            mapRt = map.get_tile_rect(playerPt);
            if ((IntersectRect(&rcTemp, &playerRt, &mapRt) && map.get_object(playerPt) != WHITE) || playerPos.x < mapSize.left)
                isIntersect = TRUE;
            playerPt.y++;
        }
        break;
    case RIGHT:
        playerPos.x += PLAYER_SIZE;
        playerPt = position_to_point(playerPos);
        playerPt.y -= 1;
        for (int i = 0; i < 3; ++i)
        {
            mapRt = map.get_tile_rect(playerPt);
            if ((IntersectRect(&rcTemp, &playerRt, &mapRt) && map.get_object(playerPt) != WHITE) || playerPos.x > mapSize.right)
                isIntersect = TRUE;
            playerPt.y++;
        }
        break;
    case UP:
        playerPos.y -= PLAYER_SIZE;
        playerPt = position_to_point(playerPos);
        playerPt.x -= 1;
        for (int i = 0; i < 3; ++i)
        {
            mapRt = map.get_tile_rect(playerPt);
            if ((IntersectRect(&rcTemp, &playerRt, &mapRt) && map.get_object(playerPt) != WHITE) || playerPos.y < mapSize.top)
                isIntersect = TRUE;
            playerPt.x++;
        }
        break;
    case DOWN:
        playerPos.y += PLAYER_SIZE;
        playerPt = position_to_point(playerPos);
        playerPt.x -= 1;
        for (int i = 0; i < 3; ++i)
        {
            mapRt = map.get_tile_rect(playerPt);
            if ((IntersectRect(&rcTemp, &playerRt, &mapRt) && map.get_object(playerPt) != WHITE) || playerPos.y > mapSize.bottom)
                isIntersect = TRUE;
            playerPt.x++;
        }
        break;
    }

    return isIntersect;
}

void bullet_shoot(Player *_player, Bullet _bullet[], int _idx, int _direction)
{
    BulletInfo info;
    info = _player->shoot(_direction);
    if (info._direction != -1)
        _bullet[_idx].make_bullet(info);
}

BOOL intersect_bullet_to_map(Bullet *_bullet, Map *_map)
{
    POINT bulletPos = _bullet->get_position();
    POINT bulletPt = position_to_point(bulletPos);
    RECT mapSize = _map->get_map_size();
    if (mapSize.left >= bulletPos.x || mapSize.right <= bulletPos.x || mapSize.top >= bulletPos.y || mapSize.bottom <= bulletPos.y)
    {
        _bullet->dead();
        return TRUE;
    }
    else if (_map->get_object(bulletPt) == RED)
    {
        _bullet->dead();
        return TRUE;
    }
    else if (_map->get_object(bulletPt) == BLACK)
    {
        _map->tile_change_white(bulletPt);
        return TRUE;
    }
    return FALSE;
}