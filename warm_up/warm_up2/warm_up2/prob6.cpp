#include <iostream>

using namespace std;

void drawX(int width);
void drawRevTriangle(int width);
void drawRhombus(int width);
void drawButterfly(int width);
void drawHourglass(int width);
void drawRectangle(int width);

int main(void)
{
    int shape;
    int width;

    cin >> shape >> width;

    while (width % 2 != 0)
    {
        cout << "You can only enter even number!";
        cin >> width;
    }

    switch (shape)
    {
    case 1:
        drawX(width);
        break;
    case 2:
        drawRevTriangle(width);
        break;
    case 3:
        drawRhombus(width);
        break;
    case 4:
        drawButterfly(width);
        break;
    case 5:
        drawHourglass(width);
        break;
    case 6:
        drawRectangle(width);
        break;

    default:
        cout << "Please enter number 1~6!";
        break;
    }

    return 0;
}

void drawX(int width)
{
    int blank = 0;
    int blank2 = width / 2;

    while (blank2 > 1)
    {
        for (int i = 0; i < blank; ++i)
            cout << " ";
        cout << "*";
        for (int i = 0; i < blank2; ++i)
            cout << " ";
        cout << "*\n";

        ++blank;
        blank2 -= 2;
    }

    while (blank2 <= (width / 2))
    {
        for (int i = 0; i < blank; ++i)
            cout << " ";
        cout << "*";
        for (int i = 0; i < blank2; ++i)
            cout << " ";
        cout << "*\n";

        --blank;
        blank2 += 2;
    }
}

void drawRevTriangle(int width)
{
    int blank = 0;
    for (int i = 0; i < (width / 2); ++i)
    {
        for (int j = 0; j < blank; ++j)
        {
            cout << " ";
        }
        for (int k = 0; k < (width - blank * 2); ++k)
        {
            cout << "*";
        }
        cout << "\n";
        ++blank;
    }
}

void drawRhombus(int width)
{
    int blank = width / 2 - 1;
    int asterisk = 2;

    while (asterisk < width)
    {
        for (int i = 0; i < blank; ++i)
            cout << " ";
        for (int i = 0; i < asterisk; ++i)
            cout << "*";
        cout << "\n";
        blank -= 2;
        asterisk += 4;
    }

    while (asterisk >= 2)
    {
        for (int i = 0; i < blank; ++i)
            cout << " ";
        for (int i = 0; i < asterisk; ++i)
            cout << "*";
        cout << "\n";
        blank += 2;
        asterisk -= 4;
    }
}

void drawButterfly(int width)
{
    int blank = width - 2;
    int asterisk = 1;

    while (asterisk < width / 2)
    {
        for (int i = 0; i < asterisk; ++i)
            cout << "*";
        for (int i = 0; i < blank; ++i)
            cout << " ";
        for (int i = 0; i < asterisk; ++i)
            cout << "*";
        cout << "\n";
        blank -= 4;
        asterisk += 2;
    }

    if (asterisk != width / 2)
    {
        for (int i = 0; i < (asterisk - 1) * 2; ++i)
            cout << "*";
        cout << "\n";
        blank += 4;
        asterisk -= 2;
    }

    while (asterisk > 0)
    {
        for (int i = 0; i < asterisk; ++i)
            cout << "*";
        for (int i = 0; i < blank; ++i)
            cout << " ";
        for (int i = 0; i < asterisk; ++i)
            cout << "*";
        cout << "\n";
        blank += 4;
        asterisk -= 2;
    }
}

void drawHourglass(int width)
{

    int asterisk = width;
    int blank = 0;

    while (asterisk > 2)
    {
        for (int i = 0; i < blank; ++i)
        {
            cout << " ";
        }
        for (int i = 0; i < asterisk; ++i)
        {
            cout << "*";
        }
        cout << "\n";
        blank += 2;
        asterisk -= 4;
    }

    if (asterisk == 0)
    {
        for (int i = 0; i < width / 2 - 1; ++i)
            cout << " ";
        cout << "**\n";
        blank -= 2;
        asterisk += 4;
    }

    while (asterisk <= width)
    {
        for (int i = 0; i < blank; ++i)
        {
            cout << " ";
        }
        for (int i = 0; i < asterisk; ++i)
        {
            cout << "*";
        }
        cout << "\n";
        blank -= 2;
        asterisk += 4;
    }
}

void drawRectangle(int width)
{
    for (int i = 0; i < (width / 2); ++i)
    {
        if (i == 0 || i == (width / 2 - 1))
            for (int j = 0; j < width; ++j)
            {
                cout << "*";
            }
        else
        {
            cout << "*";
            for (int j = 0; j < width - 2; ++j)
                cout << " ";
            cout << "*";
        }
        cout << "\n";
    }
}