#include <iostream>
#include <windows.h>
#include <random>

std::random_device re;
std::mt19937 gen(re());
std::uniform_int_distribution<int> loc(0, 9);
std::uniform_int_distribution<int> sta(0, 4);

typedef struct _STONE
{
    int row, column;
    char state;
} STONE;

void stoneInit(STONE*, STONE*);
void drawField(STONE*, STONE*, bool);
void moveUp(STONE*);
void moveDown(STONE*);
void moveLeft(STONE*);
void moveRight(STONE*);
void changeState(STONE*);
bool crashCheck(STONE*, STONE*);

int main()
{
    STONE player1;
    STONE player2;
    STONE* turnPlayer = nullptr;
    STONE* otherPlayer = nullptr;
    bool isCrash = false;

    stoneInit(&player1, &player2);

    turnPlayer = &player1;
    otherPlayer = &player2;

    drawField(turnPlayer, otherPlayer, isCrash);

    char order = 0;

    while (order != 'q')
    {
        std::cin >> order;
        switch (order)
        {
        case 'r':
        {
            stoneInit(&player1, &player2);
            turnPlayer = &player1;
            otherPlayer = &player2;
            drawField(turnPlayer, otherPlayer, isCrash);
            bool crashCheck = false;
            break;
        }
        case 'w':
        {
            if (turnPlayer == &player1)
            {
                moveUp(turnPlayer);
                isCrash = crashCheck(turnPlayer, otherPlayer);
                drawField(turnPlayer, otherPlayer, isCrash);
                turnPlayer = &player2;
                otherPlayer = &player1;
            }
            else {
                std::cout << "잘못된 플레이어 입니다.";
            }
            break;
        }
        case 'a':
        {
            if (turnPlayer == &player1)
            {
                moveLeft(turnPlayer);
                isCrash = crashCheck(turnPlayer, otherPlayer);
                drawField(turnPlayer, otherPlayer, isCrash);
                turnPlayer = &player2;
                otherPlayer = &player1;
            }
            else {
                std::cout << "잘못된 플레이어 입니다.";
            }
            break;
        }
        case 's':
        {
            if (turnPlayer == &player1)
            {
                moveDown(turnPlayer);
                isCrash = crashCheck(turnPlayer, otherPlayer);
                drawField(turnPlayer, otherPlayer, isCrash);
                turnPlayer = &player2;
                otherPlayer = &player1;
            }
            else {
                std::cout << "잘못된 플레이어 입니다.";
            }
            break;
        }
        case 'd':
        {
            if (turnPlayer == &player1)
            {
                moveRight(turnPlayer);
                isCrash = crashCheck(turnPlayer, otherPlayer);
                drawField(turnPlayer, otherPlayer, isCrash);
                turnPlayer = &player2;
                otherPlayer = &player1;
            }
            else {
                std::cout << "잘못된 플레이어 입니다.";
            }
            break;
        }
        case 'i':
        {
            if (turnPlayer == &player2)
            {
                moveUp(turnPlayer);
                isCrash = crashCheck(turnPlayer, otherPlayer);
                drawField(turnPlayer, otherPlayer, isCrash);
                turnPlayer = &player1;
                otherPlayer = &player2;
            }
            else {
                std::cout << "잘못된 플레이어 입니다.";
            }
            break;
        }
        case 'j':
        {
            if (turnPlayer == &player2)
            {
                moveLeft(turnPlayer);
                isCrash = crashCheck(turnPlayer, otherPlayer);
                drawField(turnPlayer, otherPlayer, isCrash);
                turnPlayer = &player1;
                otherPlayer = &player2;
            }
            else {
                std::cout << "잘못된 플레이어 입니다.";
            }
            break;
        }
        case 'k':
        {
            if (turnPlayer == &player2)
            {
                moveDown(turnPlayer);
                isCrash = crashCheck(turnPlayer, otherPlayer);
                drawField(turnPlayer, otherPlayer, isCrash);
                turnPlayer = &player1;
                otherPlayer = &player2;
            }
            else {
                std::cout << "잘못된 플레이어 입니다.";
            }
            break;
        }
        case 'l':
        {
            if (turnPlayer == &player2)
            {
                moveRight(turnPlayer);
                isCrash = crashCheck(turnPlayer, otherPlayer);
                drawField(turnPlayer, otherPlayer, isCrash);
                turnPlayer = &player1;
                otherPlayer = &player2;
            }
            else {
                std::cout << "잘못된 플레이어 입니다.";
            }
            break;
        }
        }
    }
}

void stoneInit(STONE* player1, STONE* player2)
{
    player1->state = 'o';
    player2->state = 'x';

    int randRow1, randRow2, randCol1, randCol2;
    randRow1 = loc(gen);
    randRow2 = loc(gen);
    while (randRow1 == randRow2)
    {
        randRow1 = loc(gen);
    }

    randCol1 = loc(gen);
    randCol2 = loc(gen);

    player1->row = randRow1;
    player1->column = randCol1;

    player2->row = randRow2;
    player2->column = randCol2;
}

void drawField(STONE* turnPlayer, STONE* otherPlayer, bool isCrash)
{
    system("cls");
    if (isCrash == false)
    {
        for (int i = 0; i < 10; ++i)
        {
            std::cout << "_______________________________\n";
            for (int j = 0; j < 10; ++j)
            {
                std::cout << "|";
                if (turnPlayer->row == i && turnPlayer->column == j)
                {
                    std::cout << turnPlayer->state;
                }
                else
                    std::cout << " ";
                if (otherPlayer->row == i && otherPlayer->column == j)
                {
                    std::cout << otherPlayer->state;
                }
                else
                    std::cout << " ";
            }
            std::cout << "|\n";
        }
        std::cout << "_______________________________\n";
    }
    else
    {
        Beep(1000, 50);
        for (int i = 0; i < 10; ++i)
        {
            std::cout << "_______________________________\n";
            for (int j = 0; j < 10; ++j)
            {
                std::cout << "| ";
                if (turnPlayer->row == i && turnPlayer->column == j)
                {
                    std::cout << turnPlayer->state;
                }
                else
                    std::cout << " ";
            }
            std::cout << "|\n";
        }
        std::cout << "_______________________________\n";
    }
}

void moveUp(STONE* stone)
{
    stone->row -= 1;
    if (stone->row < 0)
    {
        stone->row = 9;
        changeState(stone);
    }
}

void moveDown(STONE* stone)
{
    stone->row += 1;
    if (stone->row > 9)
    {
        stone->row = 0;
        changeState(stone);
    }
}

void moveLeft(STONE* stone)
{
    stone->column -= 1;
    if (stone->column < 0)
    {
        stone->column = 9;
        changeState(stone);
    }
}

void moveRight(STONE* stone)
{
    stone->column += 1;
    if (stone->column > 9)
    {
        stone->column = 0;
        changeState(stone);
    }
}

void changeState(STONE* stone)
{
    int randState = sta(gen);
    switch (randState)
    {
    case 0:
        stone->state = 'o';
        break;
    case 1:
        stone->state = 'x';
        break;
    case 2:
        stone->state = '#';
        break;
    case 3:
        stone->state = '@';
        break;
    case 4:
        stone->state = '*';
        break;
    }
}

bool crashCheck(STONE* player1, STONE* player2)
{
    if (player1->row == player2->row && player1->column == player2->column)
        return true;
    else
        return false;
}
