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
void intersect_enemy_to_map(Enemy *, Map *);

BOOL intersect_enemy_to_player(Player *, Enemy *);

BOOL intersect_bullet_to_enemy(Bullet *, Enemy *, Map *);
BOOL enemy_dead_chain(Enemy *, int, Map *);

void player_move(Player *, Map *, int);

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
    HFONT hFont, oldFont;
    HPEN hPen, oldPen;
    HBRUSH hBrush, oldBrush;

    static BOOL isMainMenu;
    static BOOL isGameStart;
    static BOOL isGameOver;

    static Map map;
    static Player player;

    static int level;

    static Bullet bullet[MAX_BULLET_DRAW];
    static int bulletIdx;
    static int bulletCnt;
    static int bulletBonus;

    static Enemy enemy[MAX_ENEMY];
    static int enemyIdx;
    static int enemySpawnCount;
    static int enemySpawnTime = 30;

    static BOOL keyBuffer[256];

    static RECT buttonLevelEasy;
    static RECT buttonLevelNormal;
    static RECT buttonLevelHard;
    static RECT buttonExit;

    static RECT buttonMainMenu;

    static RECT uiBox;
    static RECT lifeBox;
    static RECT scoreBox;
    static int score;
    static TCHAR scoreText[100];

    SIZE size;
    BulletInfo info;
    RECT tempRT;
    POINT tempPT;
    POINT tempPT2;
    int tempObj;
    RECT tempMapRT;
    RECT tempPlayerRT;
    int col, row;
    std::vector<POINT> spawnPt;
    int randNum;
    int mx, my;
    POINT mouse;

    static BOOL isShoot;
    int bulletDirection;

    switch (iMessage)
    {
    case WM_CREATE:
        isMainMenu = TRUE;
        isGameStart = FALSE;
        isGameOver = FALSE;

        score = 0;
        wsprintf(scoreText, L"Score : %d", score);

        GetClientRect(hWnd, &tempRT);
        tempRT.left = tempRT.right / 7;
        tempRT.top = tempRT.bottom / 5;

        buttonLevelEasy = {
            tempRT.left * 2,
            tempRT.top * 2,
            tempRT.left * 3,
            tempRT.top * 3,
        };
        buttonLevelNormal = {
            tempRT.left * 3,
            tempRT.top * 2,
            tempRT.left * 4,
            tempRT.top * 3,
        };
        buttonLevelHard = {
            tempRT.left * 4,
            tempRT.top * 2,
            tempRT.left * 5,
            tempRT.top * 3,
        };
        buttonExit = {
            tempRT.left * 3,
            tempRT.top * 3,
            tempRT.left * 4,
            tempRT.top * 4,
        };

        bulletIdx = 0;
        bulletCnt = 0;
        bulletBonus = 0;
        enemySpawnCount = 30;
        isShoot = FALSE;
        level = map.get_level();
        for (int i = 0; i < 256; ++i)
            keyBuffer[i] = FALSE;
        break;

    case WM_TIMER:
        switch (wParam)
        {
        case BULLET_RENDER:
            player.in_bullet_rotate();
            InvalidateRect(hWnd, NULL, FALSE);
            break;

        case BULLET_MOVE:
            for (int i = 0; i < MAX_BULLET_DRAW; ++i)
            {
                if (bullet[i].get_is_live() && bullet[i].get_is_shoot())
                {
                    bullet[i].move();
                    if (intersect_bullet_to_map(&bullet[i], &map))
                    {
                        std::cout << "이건 불림?\n";
                        --bulletCnt;
                    }
                    if (intersect_bullet_to_enemy(&bullet[i], enemy, &map))
                    {
                        std::cout << "안불림\n";
                        if (bulletIdx++ == MAX_BULLET_DRAW)
                            bulletIdx = 0;
                        --bulletCnt;
                        score += 100;
                        wsprintf(scoreText, L"Score : %d", score);
                    }
                }
            }
            InvalidateRect(hWnd, NULL, FALSE);
            break;

        case BULLET_RELOAD:
            player.bullet_reload();
            break;

        case ENEMY_SPAWN:
            tempMapRT = map.get_map_size();
            if (player.get_is_live())
                if (enemySpawnCount == 0)
                {
                    for (int i = 0; i < MAX_ENEMY; ++i)
                    {
                        if (!enemy[i].get_is_live())
                        {
                            enemy[i].enemy_spawn(tempMapRT);
                            enemySpawnCount = enemySpawnTime;
                            if (enemySpawnTime >= 10)
                                --enemySpawnTime;
                            break;
                        }
                    }
                }
                else
                    enemySpawnCount--;
            InvalidateRect(hWnd, NULL, FALSE);
            break;

        case OBJECT_MOVE:
            if (keyBuffer[0x41])
            {
                player_move(&player, &map, LEFT);
                player_move(&player, &map, LEFT);
                player_move(&player, &map, LEFT);
            }
            if (keyBuffer[0x44])
            {
                player_move(&player, &map, RIGHT);
                player_move(&player, &map, RIGHT);
                player_move(&player, &map, RIGHT);
            }
            if (keyBuffer[0x57])
            {
                player_move(&player, &map, UP);
                player_move(&player, &map, UP);
                player_move(&player, &map, UP);
            }
            if (keyBuffer[0x53])
            {
                player_move(&player, &map, DOWN);
                player_move(&player, &map, DOWN);
                player_move(&player, &map, DOWN);
            }

            tempPT = player.get_position();
            if (player.get_is_live())
                for (int i = 0; i < MAX_ENEMY; ++i)
                {
                    if (enemy[i].get_is_live())
                    {
                        enemy[i].move(tempPT);
                        intersect_enemy_to_map(&enemy[i], &map);
                        if (intersect_enemy_to_player(&player, &enemy[i]))
                        {
                            enemy[i].dead();
                            enemy_dead_chain(&enemy[i], i, &map);

                            enemySpawnTime = 30;
                            SetTimer(hWnd, PLAYER_RESPAWN, RESPAWN_TIME, NULL);
                        }
                    }
                }
            InvalidateRect(hWnd, NULL, FALSE);
            break;

        case PLAYER_RESPAWN:
            if (player.get_heart() != 0)
            {
                player.heart_down();

                row = map.get_row();
                col = map.get_col();
                spawnPt.clear();
                for (int i = 1; i < row - 1; ++i)
                    for (int j = 1; j < col - 1; ++j)
                    {
                        if (map.get_object({j, i}) == WHITE)
                        {
                            spawnPt.push_back({j, i});
                        }
                    }
                randNum = rand() % spawnPt.size();
                player.init(spawnPt[randNum]);
                bulletCnt = 0;

                KillTimer(hWnd, PLAYER_RESPAWN);
                InvalidateRect(hWnd, NULL, FALSE);
            }
            else
            {
                isGameStart = FALSE;
                isGameOver = TRUE;
            }
            break;
        }
        break;

    case WM_KEYDOWN:
        if (isGameStart)
        {
            isShoot = FALSE;

            switch (wParam)
            {
            case 0x41: // a
            case 0x44: // d
            case 0x57: // w
            case 0x53: // s
                keyBuffer[wParam] = TRUE;
                for (int i = 0; i < MAX_BULLET_DRAW; ++i)
                {
                    if (bullet[i].get_is_live() && bullet[i].get_type() == SPECIAL)
                    {
                        tempPlayerRT = player.get_pos_rect();
                        tempRT = bullet[i].get_pos_rect();
                        if (IntersectRect(&tempMapRT, &tempPlayerRT, &tempRT))
                        {
                            player.pickup_special_bullet();
                            bullet[i].dead();
                        }
                    }
                }
                break;

            case VK_LEFT:
                bulletDirection = LEFT;
                isShoot = TRUE;
                break;
            case VK_RIGHT:
                bulletDirection = RIGHT;
                isShoot = TRUE;
                break;
            case VK_UP:
                bulletDirection = UP;
                isShoot = TRUE;
                break;
            case VK_DOWN:
                bulletDirection = DOWN;
                isShoot = TRUE;
                break;
            }
            if (isShoot)
            {
                std::cout << bulletCnt << "\n";
                if (bulletCnt < 1 + bulletBonus)
                {
                    std::cout << "외안돼\n";
                    bullet_shoot(&player, bullet, bulletIdx, bulletDirection);
                    ++bulletIdx;
                    if (bulletIdx == MAX_BULLET_DRAW)
                        bulletIdx = 0;
                    ++bulletCnt;
                }
            }
            isShoot = FALSE;
        }
        else if (isMainMenu)
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
                SetTimer(hWnd, BULLET_RENDER, BULLET_RENDER_TIME, NULL);
                SetTimer(hWnd, BULLET_MOVE, BULLET_MOVE_TIME, NULL);
                SetTimer(hWnd, BULLET_RELOAD, BULLET_RELOAD_TIME, NULL);
                SetTimer(hWnd, ENEMY_SPAWN, ENEMY_SPAWN_TIME, NULL);
                SetTimer(hWnd, OBJECT_MOVE, OBJECT_MOVE_TIME, NULL);
                break;
            }

        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_KEYUP:
        keyBuffer[wParam] = FALSE;
        break;

    case WM_LBUTTONDOWN:
        mx = LOWORD(lParam);
        my = HIWORD(lParam);
        mouse = {mx, my};
        if (isMainMenu)
        {
            GetClientRect(hWnd, &tempRT);
            tempRT.left = tempRT.right / 7;
            tempRT.top = tempRT.bottom / 5;
            buttonLevelEasy = {
                tempRT.left * 2,
                tempRT.top * 2,
                tempRT.left * 3,
                tempRT.top * 3,
            };
            buttonLevelNormal = {
                tempRT.left * 3,
                tempRT.top * 2,
                tempRT.left * 4,
                tempRT.top * 3,
            };
            buttonLevelHard = {
                tempRT.left * 4,
                tempRT.top * 2,
                tempRT.left * 5,
                tempRT.top * 3,
            };
            buttonExit = {
                tempRT.left * 3,
                tempRT.top * 3,
                tempRT.left * 4,
                tempRT.top * 4,
            };

            if (PtInRect(&buttonLevelEasy, mouse))
            {
                map.change(EASY);
                isGameStart = TRUE;
            }
            if (PtInRect(&buttonLevelNormal, mouse))
            {
                map.change(NORMAL);
                isGameStart = TRUE;
            }
            if (PtInRect(&buttonLevelHard, mouse))
            {
                map.change(HARD);
                isGameStart = TRUE;
            }
            if (PtInRect(&buttonExit, mouse))
            {
                PostQuitMessage(0);
            }
            if (isGameStart)
            {
                isMainMenu = FALSE;
                player.init(map.get_level());
                SetTimer(hWnd, BULLET_RENDER, BULLET_RENDER_TIME, NULL);
                SetTimer(hWnd, BULLET_MOVE, BULLET_MOVE_TIME, NULL);
                SetTimer(hWnd, BULLET_RELOAD, BULLET_RELOAD_TIME, NULL);
                SetTimer(hWnd, ENEMY_SPAWN, ENEMY_SPAWN_TIME, NULL);
                SetTimer(hWnd, OBJECT_MOVE, OBJECT_MOVE_TIME, NULL);
            }
        }
        else if (isGameOver)
        {
            GetClientRect(hWnd, &tempRT);
            tempRT.bottom = tempRT.bottom / 5;
            buttonMainMenu = tempRT;
            buttonMainMenu.top = tempRT.bottom * 2;
            buttonMainMenu.bottom = tempRT.bottom * 3;

            if (PtInRect(&buttonMainMenu, mouse))
            {
                isGameOver = FALSE;
                isMainMenu = TRUE;
            }
        }
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &bufferRT);
        mdc = CreateCompatibleDC(hdc);
        backBit = CreateCompatibleBitmap(hdc, bufferRT.right, bufferRT.bottom);
        SelectObject(mdc, (HBITMAP)backBit);
        PatBlt(mdc, 0, 0, bufferRT.right, bufferRT.bottom, WHITENESS);

        if (isMainMenu)
        {
            hFont = CreateFont(50, 0, 0, 0, FW_HEAVY, 1, 0, 0, 0, 0, 0, 0, 0, 0);
            oldFont = (HFONT)SelectObject(mdc, hFont);
            GetClientRect(hWnd, &tempRT);
            tempRT.top = (tempRT.bottom + tempRT.top) / 5;
            DrawText(mdc, L"INVERSUS", 8, &tempRT, DT_CENTER);
            SelectObject(mdc, oldFont);
            DeleteObject(hFont);

            GetClientRect(hWnd, &tempRT);
            tempRT.left = tempRT.right / 7;
            tempRT.top = tempRT.bottom / 5;
            buttonLevelEasy = {
                tempRT.left * 2,
                tempRT.top * 2,
                tempRT.left * 3,
                tempRT.top * 3,
            };
            buttonLevelNormal = {
                tempRT.left * 3,
                tempRT.top * 2,
                tempRT.left * 4,
                tempRT.top * 3,
            };
            buttonLevelHard = {
                tempRT.left * 4,
                tempRT.top * 2,
                tempRT.left * 5,
                tempRT.top * 3,
            };
            buttonExit = {
                tempRT.left * 3,
                tempRT.top * 3,
                tempRT.left * 4,
                tempRT.top * 4,
            };
            hFont = CreateFont(20, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            oldFont = (HFONT)SelectObject(mdc, hFont);
            hBrush = CreateSolidBrush(RGB(0, 0, 0));
            oldBrush = (HBRUSH)SelectObject(mdc, hBrush);

            DrawText(mdc, L"EASY", 4, &buttonLevelEasy, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            // FrameRect(mdc, &buttonLevelEasy, hBrush);
            DrawText(mdc, L"NORMAL", 6, &buttonLevelNormal, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            // FrameRect(mdc, &buttonLevelNormal, hBrush);
            DrawText(mdc, L"HARD", 4, &buttonLevelHard, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            // FrameRect(mdc, &buttonLevelHard, hBrush);
            DrawText(mdc, L"EXIT", 4, &buttonExit, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            // FrameRect(mdc, &buttonExit, hBrush);

            SelectObject(mdc, oldBrush);
            DeleteObject(hBrush);
            SelectObject(mdc, oldFont);
            DeleteObject(hFont);
        }
        else if (isGameStart)
        {
            row = map.get_row();
            col = map.get_col();

            map.draw(mdc);

            if (player.get_is_live())
                player.draw(mdc);
            else
                player.dead_effect(mdc);

            for (int i = 0; i < MAX_ENEMY; ++i)
                if (enemy[i].get_is_live())
                {
                    enemy[i].draw(mdc);
                    tempPT = enemy[i].get_point();
                    --tempPT.x;
                    --tempPT.y;
                    for (int k = 0; k < 3; ++k)
                        for (int j = 0; j < 3; ++j)
                        {
                            tempPT2 = {tempPT.x + k, tempPT.y + j};
                            if (tempPT2.x == 0 || tempPT2.x == col - 1 || tempPT2.y == 0 || tempPT2.y == row - 1)
                                continue;
                            enemy[i].hatch_draw(mdc, map.get_tile_rect(tempPT2));
                        }
                }
                else
                    enemy[i].dead_effect(mdc);

            for (int i = 0; i < MAX_BULLET_DRAW; ++i)
                if (bullet[i].get_is_live())
                    bullet[i].draw(mdc);

            hBrush = CreateSolidBrush(RGB(20, 20, 20));
            oldBrush = (HBRUSH)SelectObject(mdc, hBrush);
            GetClientRect(hWnd, &tempRT);
            uiBox = tempRT;
            uiBox.bottom = MAP_LOCATION + 30;
            Rectangle(mdc, uiBox.left, uiBox.top, uiBox.right, uiBox.bottom);
            SelectObject(mdc, oldBrush);
            DeleteObject(hBrush);

            hFont = CreateFont(30, 0, 0, 0, FW_HEAVY, 1, 0, 0, 0, 0, 0, 0, 0, 0);
            oldFont = (HFONT)SelectObject(mdc, hFont);
            SetBkColor(mdc, RGB(20, 20, 20));
            SetTextColor(mdc, RGB(255, 255, 255));
            DrawText(mdc, L"LIFE :", 6, &uiBox, DT_SINGLELINE | DT_VCENTER);

            hBrush = CreateSolidBrush(RGB(200, 200, 200));
            oldBrush = (HBRUSH)SelectObject(mdc, hBrush);
            GetTextExtentPoint32(mdc, L"LIFE :", 7, &size);
            lifeBox = {
                size.cx,
                uiBox.bottom / 2 - PLAYER_SIZE,
                size.cx + PLAYER_SIZE * 2,
                uiBox.bottom / 2 + PLAYER_SIZE,
            };
            for (int i = 0; i < player.get_heart(); ++i)
            {
                RoundRect(mdc, lifeBox.left, lifeBox.top, lifeBox.right, lifeBox.bottom, 5, 5);
                OffsetRect(&lifeBox, PLAYER_SIZE * 2, 0);
            }
            SelectObject(mdc, oldBrush);
            DeleteObject(hBrush);

            SetBkColor(mdc, RGB(20, 20, 20));
            SetTextColor(mdc, RGB(255, 255, 255));
            DrawText(mdc, scoreText, lstrlen(scoreText), &uiBox, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

            SelectObject(mdc, oldFont);
            DeleteObject(hFont);
        }
        else if (isGameOver)
        {
            hFont = CreateFont(50, 0, 0, 0, FW_HEAVY, 1, 0, 0, 0, 0, 0, 0, 0, 0);
            oldFont = (HFONT)SelectObject(mdc, hFont);
            GetClientRect(hWnd, &tempRT);
            tempRT.bottom = tempRT.bottom / 5;
            DrawText(mdc, L"GAME OVER", 9, &tempRT, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
            SelectObject(mdc, oldFont);
            DeleteObject(hFont);

            scoreBox = tempRT;
            scoreBox.top = tempRT.bottom;
            scoreBox.bottom = tempRT.bottom * 2;

            hFont = CreateFont(40, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
            oldFont = (HFONT)SelectObject(mdc, hFont);
            DrawText(mdc, scoreText, lstrlen(scoreText), &scoreBox, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
            SelectObject(mdc, oldFont);
            DeleteObject(hFont);

            hFont = CreateFont(30, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            oldFont = (HFONT)SelectObject(mdc, hFont);
            buttonMainMenu = tempRT;
            buttonMainMenu.top = tempRT.bottom * 2;
            buttonMainMenu.bottom = tempRT.bottom * 3;
            DrawText(mdc, L"Main Menu", 9, &buttonMainMenu, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

            SelectObject(mdc, oldFont);
            DeleteObject(hFont);
        }

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

    int row = map.get_row();
    int col = map.get_col();

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
    int type = _map->get_object(bulletPt);
    if (type == WALL || type == RED || type == -1)
    {
        _bullet->dead();
        return TRUE;
    }
    else if (type == BLACK)
    {
        _map->tile_change_white(bulletPt);
    }
    return FALSE;
}

BOOL intersect_bullet_to_enemy(Bullet *bullet, Enemy *enemy, Map *map)
{
    RECT enemyRt;
    for (int i = 0; i < MAX_ENEMY; ++i)
    {
        if (enemy[i].get_is_live())
        {
            enemyRt = enemy[i].get_pos_rect();
            if (PtInRect(&enemyRt, bullet->get_position()))
            {
                bullet->dead();
                BulletInfo info = enemy[i].drop_bullet();
                bullet->make_special_bullet(info);
                enemy[i].dead();
                return TRUE;
            }
        }
    }
    return FALSE;
}

BOOL enemy_dead_chain(Enemy *enemy, int idx, Map *map)
{
    BOOL isChain = FALSE;

    POINT deadPt, enemyPt, tempPt;
    RECT deadRt;

    int row = map->get_row();
    int col = map->get_col();

    tempPt = enemy[idx].get_point();
    --tempPt.x;
    --tempPt.y;
    for (int k = 0; k < 3; ++k)
        for (int j = 0; j < 3; ++j)
        {
            deadPt = {tempPt.x + k, tempPt.y + j};
            if (deadPt.x <= 0 || deadPt.x >= col || deadPt.y <= 0 || deadPt.y >= row)
                continue;
            map->tile_change_white(deadPt);
            deadRt = map->get_tile_rect(deadPt);
            for (int i = 0; i < MAX_ENEMY; ++i)
                if (enemy[i].get_is_live())
                {
                    enemyPt = enemy[i].get_position();
                    if (PtInRect(&deadRt, enemyPt))
                    {
                        enemy[i].dead();
                        enemy_dead_chain(&enemy[i], i, map);
                        isChain = TRUE;
                    }
                }
        }
    return isChain;
}

void intersect_enemy_to_map(Enemy *enemy, Map *map)
{
    if (map->get_object(enemy->get_point()) == WHITE)
        map->tile_change_black(enemy->get_point());
}

BOOL intersect_enemy_to_player(Player *player, Enemy *enemy)
{
    RECT tempRt;
    RECT playerRt = player->get_pos_rect();
    RECT enemyRt = enemy->get_pos_rect();
    if (IntersectRect(&tempRt, &playerRt, &enemyRt))
    {
        player->dead();
        return TRUE;
    }
    return FALSE;
}

void player_move(Player *player, Map *map, int direction)
{
    if (!intersect_player_to_map(*player, *map, direction))
        player->move(direction, map->get_map_size());
    switch (direction)
    {
    case LEFT:
        if (intersect_player_to_map(*player, *map, direction))
            player->move(RIGHT, map->get_map_size());
        break;
    case RIGHT:
        if (intersect_player_to_map(*player, *map, direction))
            player->move(LEFT, map->get_map_size());
        break;
    case UP:
        if (intersect_player_to_map(*player, *map, direction))
            player->move(DOWN, map->get_map_size());
        break;
    case DOWN:
        if (intersect_player_to_map(*player, *map, direction))
            player->move(UP, map->get_map_size());
        break;
    }
}

// TODO
// 적 죽을 때 특수 탄 드랍 및 플레이어 특수탄 획득, 사용
// 적 죽으면 일정시간 콤보 활성화 화면 상단에 띄우기
// 무적 모드 단축키 생성
// 전부 만들고 나서 리드미 파일 만들기