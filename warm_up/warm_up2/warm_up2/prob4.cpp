#include <iostream>
#include <cstring>

using namespace std;

const char *week[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

int monthDays(int year, int month);
void dateOfWeek(int year, int month, int day);
void showCalendar(int year, int month);
void showFriday13(void);

int main(void)
{
    int year = 0;
    int month = 0;
    int day = 0;

    char order[10];

    while (true)
    {
        year = 0;
        month = 0;
        day = 0;

        cout << "날짜(0000/00/00) : 요일 및 달력 출력\n f/F : 13일의 금요일 출력\n q : 프로그램 종료\n";
        cin >> order;

        if (order[0] == 'q')
            break;

        if (order[0] == 'f' || order[0] == 'F')
        {
            showFriday13();
            continue;
        }

        int i = 0;

        for (i; i < strlen(order); ++i)
        {
            if (order[i] == '/')
                break;
            year = year * 10 + order[i] - 48;
        }

        for (++i; i < strlen(order); ++i)
        {
            if (order[i] == '/')
                break;
            month = month * 10 + order[i] - 48;
        }

        for (++i; i < strlen(order); ++i)
        {
            if (order[i] == '/')
                break;
            day = day * 10 + order[i] - 48;
        }

        dateOfWeek(year, month, day);
        showCalendar(year, month);
    }

    return 0;
}

int monthDays(int year, int month)
{
    switch (month)
    {
    case 2:
        if (year % 4 == 0 && year % 100 != 0 || year % 400 == 0)
            return 29;
        else
            return 28;

    case 4:
    case 6:
    case 9:
    case 11:
        return 30;

    default:
        return 31;
    }
}

void dateOfWeek(int year, int month, int day)
{
    int preYear = year - 1;
    int dayNumber = preYear * 365 + (preYear / 4 - preYear / 100 + preYear / 400);

    for (int i = 0; i < month - 1; ++i)
    {
        dayNumber += monthDays(year, (i + 1));
    }

    dayNumber += day;

    cout << week[dayNumber % 7] << "\n";
}

void showCalendar(int year, int month)
{
    int preYear = year - 1;
    int dayNumber = preYear * 365 + (preYear / 4 - preYear / 100 + preYear / 400);

    for (int i = 0; i < month - 1; ++i)
    {
        dayNumber += monthDays(year, (i + 1));
    }

    ++dayNumber;

    cout << "\t" << year << "-" << month << "\n";
    for (int i = 0; i < 7; ++i)
    {
        cout << week[i] << "\t";
    }

    cout << "\n";

    for (int i = 0; i < (dayNumber % 7); ++i)
    {
        cout << "\t";
    }

    for (int i = 0; i < monthDays(year, month); ++i)
    {
        if ((dayNumber + i) % 7 == 0)
        {
            cout << "\n";
        }
        cout << i + 1 << "\t";
    }

    cout << "\n";
}

void showFriday13(void)
{
    for (int year = 2020; year < 2031; ++year)
    {
        int preYear = year - 1;
        int dayNumber = preYear * 365 + (preYear / 4 - preYear / 100 + preYear / 400);

        ++dayNumber;

        for (int month = 0; month < 12; ++month)
        {
            if (dayNumber % 7 == 0)
            {
                cout << year << "-" << month + 1 << "-13\n";
            }

            dayNumber += monthDays(year, month + 1);
        }
    }
}