#include <iostream>
#include <string>

using namespace std;

typedef struct _MEMBER
{
    string name;
    int id;
    int birth[2];
    int attend[6];
} MEMBER;

void newMember(MEMBER *list, int listCount);
void showList(MEMBER *list);
int sumAttend(MEMBER *list, int n);

int main(void)
{
    MEMBER list[15];
    int listCount = 0;
    bool listisFull = false;

    list[listCount++] = {"Song", 123, {03, 02}, {10, 10, 15, 5, 25, 20}};
    list[listCount++] = {"Kim", 213, {01, 22}, {5, 8, 10, 20, 20, 21}};
    list[listCount++] = {"Park", 325, {9, 9}, {1, 2, 3, 4, 5, 6}};

    char order = 0;

    while (order != 'q')
    {
        cin >> order;
        switch (order)
        {
        case 'a':
        {
            if (listisFull == true)
            {
                cout << "더 이상 저장할 수 없습니다.";
                break;
            }
            newMember(list, listCount++);
            if (listCount == 15)
            {
                listisFull = true;
                --listCount;
            }
            break;
        }
        case 'p':
        {
            for (int i = 0; i < listCount; ++i)
                showList(&list[i]);
            break;
        }
        case 'm':
        {
            MEMBER *max = &list[0];
            for (int i = 0; i < listCount - 1; ++i)
            {
                if (sumAttend(&list[i], 6) < sumAttend(&list[i + 1], 6))
                    max = &list[i + 1];
            }
            showList(max);
            break;
        }
        case 'n':
        {
            MEMBER *min = &list[0];
            for (int i = 0; i < listCount - 1; ++i)
            {
                if (sumAttend(&list[i], 6) > sumAttend(&list[i + 1], 6))
                    min = &list[i + 1];
            }
            showList(min);
            break;
        }
        case '1':
        case '2':
        case '3':
        {
            int mon = order - 49;
            MEMBER *max = &list[0];
            for (int i = 0; i < listCount - 1; ++i)
            {
                if (list[i].attend[mon] < list[i + 1].attend[mon])
                    max = &list[i + 1];
            }
            showList(max);
            break;
        }
        case 'i':
        {
            for (int i = 0; i < listCount - 1; ++i)
            {
                for (int j = 0; j < listCount - 1 - i; ++j)
                    if (list[j].name.compare(list[j + 1].name) == 1)
                    {
                        MEMBER temp = list[j];
                        list[j] = list[j + 1];
                        list[j + 1] = temp;
                    }
            }
            break;
        }
        case 'j':
        {
            for (int i = 0; i < listCount - 1; ++i)
            {
                for (int j = 0; j < listCount - 1 - i; ++j)
                {
                    if (list[j].id > list[j + 1].id)
                    {
                        MEMBER temp = list[j];
                        list[j] = list[j + 1];
                        list[j + 1] = temp;
                    }
                }
            }
            break;
        }

        case 'k':
        {
            for (int i = 0; i < listCount - 1; ++i)
                for (int j = 0; j < listCount - 1 - i; ++j)
                    if (list[j].birth[0] > list[j + 1].birth[0] || list[j].birth[0] == list[j + 1].birth[0] && list[j].birth[1] > list[j + 1].birth[1])
                    {
                        MEMBER temp = list[j];
                        list[j] = list[j + 1];
                        list[j + 1] = temp;
                    }
            break;
        }
        break;
        case 'q':
            cout << "프로그램 종료";
            break;
        default:
            cout << "잘못된 명령어\n";
            break;
        }
    }

    return 0;
}

void newMember(MEMBER *list, int listCount)
{
    string name;
    int id;
    int month;
    int day;
    int attend[6];

    cout << "이름 : ";
    cin >> name;
    while (name.length() > 5)
    {
        cout << "이름은 5자 이하로 입력! : ";
        cin >> name;
    }

    cout << "id : ";
    cin >> id;
    while (id < 100 || id > 999)
    {
        cout << "ID는 3자리 숫자 입력 : ";
        cin >> id;
    }

    cout << "생일 월 : ";
    cin >> month;
    while (month > 13 || month < 0)
    {
        cout << "올바른 달을 입력하세요. : ";
        cin >> month;
    }

    cout << "생일 일 : ";
    cin >> day;
    switch (month)
    {
    case 2:
        while (day < 0 || day > 29)
        {
            cout << "올바른 날짜를 입력하세요. : ";
            cin >> day;
        }
        break;
    case 4:
    case 6:
    case 9:
    case 11:
        while (day < 0 || day > 31)
        {
            cout << "올바른 날짜를 입력하세요. : ";
            cin >> day;
        }
        break;
    default:
        while (day < 0 || day > 30)
        {
            cout << "올바른 날짜를 입력하세요. : ";
            cin >> day;
        }
        break;
    }

    for (int i = 0; i < 6; ++i)
    {
        cout << i + 1 << "월달 출석 일 수 : ";
        cin >> attend[i];
        switch (i + 1)
        {
        case 2:
            if (attend[i] < 0 || attend[i] > 29)
            {
                cout << "올바른 날짜를 입력하세요.\n";
                --i;
                break;
            }
        case 4:
        case 6:
            if (attend[i] < 0 || attend[i] > 31)
            {
                cout << "올바른 날짜를 입력하세요.\n";
                --i;
                break;
            }
        default:
            if (attend[i] < 0 || attend[i] > 30)
            {
                cout << "올바른 날짜를 입력하세요.\n";
                --i;
                break;
            }
        }
    }

    list[listCount] = {name, id, {month, day}, {attend[0], attend[1], attend[2], attend[3], attend[4], attend[5]}};
}

void showList(MEMBER *list)
{
    cout << list->name << "  " << list->id << "  ";
    printf("%02d%02d  ", list->birth[0], list->birth[1]);
    for (int j = 0; j < 6; ++j)
    {
        cout << j + 1 << " ";
        cout << list->attend[j] << " ";
    }
    cout << "\n";
}

int sumAttend(MEMBER *list, int n)
{
    int sum = 0;
    for (int i = 0; i < n; ++i)
        sum += list->attend[i];
    return sum;
}