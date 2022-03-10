#include <iostream>

const int monthDays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const char *week[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Friday", "Saturday"};

void dateOfWeek(int year, int month, int day);

int main()
{
    int year;
    int month;
    int day;

    scanf_s("%d/%d/%d", &year, &month, &day);

    dateOfWeek(year, month, day);

    switch (month)
    {
    case 1:
        break;

    default:
        break;
    }
}

void dateOfWeek(int year, int month, int day)
{
    int preYear = year - 1;
    int dayNumber = preYear * 365 + (preYear / 4 - preYear / 100 + preYear / 400);

    for (int i = 0; i < month - 1; ++i)
    {
        dayNumber += monthDays[i];
    }

    if (month > 2 && (year % 4 == 0 && year % 100 != 0 || year % 400 == 0))
        ++dayNumber;

    dayNumber += day;

    std::cout << week[dayNumber % 7];
}