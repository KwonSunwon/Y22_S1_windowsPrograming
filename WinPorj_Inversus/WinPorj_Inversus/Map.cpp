#include "Map.h"

Map::Map()
{
    gameMode = 0;
}

Map::~Map()
{
    for (int i = 0; i < 20; ++i)
        delete[] map[i];
    delete[] map;
}

void Map::map_change(int _gameMode)
{
    gameMode = _gameMode;

    // for (int i = 0; i < 16; ++i)
    //     delete[] map[i];
    // delete[] map;

    switch (gameMode)
    {
    case 1:
        map = new int *[15];
        for (int i = 0; i < 16; ++i)
            map[i] = new int[16];
        for (int i = 0; i < 15; ++i)
            for (int j = 0; j < 16; ++j)
                map[i][j] = Easy_Mode[i][j];
        break;
    case 2:
        map = new int *[15];
        for (int i = 0; i < 20; ++i)
            map[i] = new int[20];
        for (int i = 0; i < 15; ++i)
            for (int j = 0; j < 20; ++j)
                map[i][j] = Normal_Mode[i][j];
        break;
    case 3:
        map = new int *[15];
        for (int i = 0; i < 20; ++i)
            map[i] = new int[20];
        for (int i = 0; i < 15; ++i)
            for (int j = 0; j < 20; ++j)
                map[i][j] = Hard_Mode[i][j];
        break;
    }
}

void Map::map_draw(HDC mdc)
{
    HBRUSH hBrush, oldBrush;

    switch (gameMode)
    {
    case 0:
        for (int i = 0; i < 16; ++i)
            for (int j = 0; j < 15; ++j)
            {
                if (map[i][j] == 0)
                {
                    hBrush = CreateSolidBrush(BLACK_BRUSH);
                    oldBrush = (HBRUSH)SelectObject(mdc, hBrush);
                    Rectangle(mdc, 40 * j, 25 * i, 40 * (j + 1), 25 * (i + 1));
                    SelectObject(mdc, oldBrush);
                    DeleteObject(hBrush);
                }
                if (map[i][j] == 1)
                {
                    hBrush = CreateSolidBrush(WHITE_BRUSH);
                    oldBrush = (HBRUSH)SelectObject(mdc, hBrush);
                    Rectangle(mdc, 40 * j, 25 * i, 40 * (j + 1), 25 * (i + 1));
                    SelectObject(mdc, oldBrush);
                    DeleteObject(hBrush);
                }
            }
        break;
    case 1:
        HBRUSH hBrush, oldBrush;
        HPEN hPen, oldPen;
        for (int i = 0; i < 20; ++i)
            for (int j = 0; j < 15; ++j)
            {
                if (Normal_Mode[j][i] == 0)
                {
                    hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
                    oldPen = (HPEN)SelectObject(mdc, hPen);
                    hBrush = CreateSolidBrush(RGB(255, 255, 255));
                    oldBrush = (HBRUSH)SelectObject(mdc, hBrush);
                    Rectangle(mdc, 40 * j, 25 * i, 40 * (j + 1), 25 * (i + 1));
                    SelectObject(mdc, oldBrush);
                    DeleteObject(hBrush);
                    SelectObject(mdc, oldPen);
                    DeleteObject(hPen);
                }
                else if (Normal_Mode[j][i] == 1)
                {
                    hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
                    oldPen = (HPEN)SelectObject(mdc, hPen);
                    hBrush = CreateSolidBrush(RGB(0, 0, 0));
                    oldBrush = (HBRUSH)SelectObject(mdc, hBrush);
                    Rectangle(mdc, 40 * j, 25 * i, 40 * (j + 1), 25 * (i + 1));
                    SelectObject(mdc, oldBrush);
                    DeleteObject(hBrush);
                    SelectObject(mdc, oldPen);
                    DeleteObject(hPen);
                }
            }
        break;
    case 2:

        break;
    }
}