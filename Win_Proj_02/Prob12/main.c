#define _CRT_SECURE_NO_WARNINGS
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include <stdio.h>

#include <Windows.h>
#include <tchar.h>
#include <time.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Window Programming Lab";

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

    hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600,
                        NULL, (HMENU)NULL, hInstance, NULL);
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

    HPEN hPen, oldPen;
    HBRUSH hBrush, oldBrush;
    HFONT hFont, oldFont;

    static RECT answerBox;

    static const TCHAR problem[5][10] = {
        {L"apple"},
        {L"secure"},
        {L"program"},
        {L"window"},
        {L"hangman"},
    };

    static int wordLength;

    static int *correct;

    static TCHAR answer[10];
    static TCHAR input[1];

    static int gameWin;
    static int gameLife;

    static int noCorrect;

    static POINT hangman[5][2] = {
        {{625, 200}, {630, 380}},
        {{625, 240}, {600, 330}},
        {{625, 240}, {650, 330}},
        {{630, 380}, {610, 460}},
        {{630, 380}, {640, 450}},
    };
    static RECT hangmanBase = {400, 500, 750, 580};
    static RECT hangmanHead = {575, 100, 675, 200};
    static POINT hangmanStick[4] = {
        {500, 500},
        {500, 50},
        {625, 50},
        {625, 100},
    };

    switch (iMessage)
    {
    case WM_CREATE:
        // 랜덤 단어 선정, 정답 배열에 저장
        srand(time(NULL));
        int randWord = rand() % 5;
        wordLength = lstrlen(problem[randWord]);

        printf("length %d\nrand %d\n", wordLength, randWord);

        for (int i = 0; i < wordLength; ++i)
        {
            answer[i] = problem[randWord][i];
        }
        answer[wordLength] = 0x00;

        correct = (int *)malloc(sizeof(int) * wordLength);

        // TEST
        //  int debugTemp;
        //  scanf("%d", &debugTemp);
        //  for (int i = 0; i < wordLength; ++i)
        //      correct[i] = debugTemp;

        for (int i = 0; i < wordLength; ++i)
            correct[i] = 0;

        input[0] = 0x00;

        gameLife = 5;
        gameWin = 0;
        noCorrect = 1;

        break;

    case WM_CHAR:
        input[0] = wParam;

        // 입력 받은 값 비교
        for (int i = 0; i < wordLength; ++i)
        {
            if (wParam == answer[i])
            {
                correct[i] = 1;
                noCorrect = 1;
            }
        }

        // 게임 승패 확인
        int check = 1;
        for (int i = 0; i < wordLength; ++i)
            if (correct[i] == 0)
                check = 0;
        if (check)
            gameWin = 1;
        else
            --gameLife;

        InvalidateRect(hWnd, NULL, TRUE);

        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

        hFont = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        oldFont = (HFONT)SelectObject(hdc, hFont);

        //[x] 회색 배경 선택
        hPen = CreatePen(PS_SOLID, 1, BLACK_PEN);
        oldPen = SelectObject(hdc, hPen);
        if (noCorrect == 0)
        {
            printf("회색 배경\n");
            hBrush = CreateSolidBrush(0xd3d3d3);
            oldBrush = SelectObject(hdc, hBrush);
            SetBkColor(hdc, 0xd3d3d3);
        }
        else
        {
            printf("흰색 배경\n");
            hBrush = CreateSolidBrush(0xffffff);
            oldBrush = SelectObject(hdc, hBrush);
        }

        RECT box = {50, 100, 90, 160};

        answerBox = box;
        for (int i = 0; i < wordLength; ++i)
        {
            Rectangle(hdc, answerBox.left, answerBox.top, answerBox.right, answerBox.bottom);
            OffsetRect(&answerBox, 50, 0);
        }
        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);

        answerBox = box;
        for (int i = 0; i < wordLength; ++i)
        {
            if (correct[i] == 1)
            {
                printf("글자 입력\n");
                TextOut(hdc, answerBox.left + 10, answerBox.top + 11, answer + i, 1);
                // DrawText(hdc, &answerBox, answer + i, 1, DT_CENTER);
            }
            answerBox.left += 50;
            answerBox.right += 50;
        }
        noCorrect = 0;

        SetBkColor(hdc, 0xffffff);

        if (gameWin == 1)
        {
            TextOut(hdc, 150, 300, L"GameWin", lstrlen(L"GameWin"));
        }
        else if (gameLife == 0)
        {
            TextOut(hdc, 150, 300, L"GameDefeat", lstrlen(L"GameDefeat"));
        }
        else
        {
            // 입력창
            TextOut(hdc, 200, 300, input, lstrlen(input));
        }
        POINT temp[2] = {{100, 330}, {330, 330}};
        Polygon(hdc, temp, 2);
        SelectObject(hdc, oldFont);

        // 행맨 그림
        FillRect(hdc, &hangmanBase, BLACK_BRUSH);
        Polyline(hdc, &hangmanStick, 4);
        Ellipse(hdc, hangmanHead.left, hangmanHead.top, hangmanHead.right, hangmanHead.bottom);
        for (int i = 0; i < 5 - gameLife; ++i)
        {
            Polygon(hdc, &hangman[i], 2);
        }

        DeleteObject(hPen);
        DeleteObject(hBrush);
        DeleteObject(hFont);

        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:

        PostQuitMessage(0);
        return 0;
    }
    return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

// 1. 랜덤한 단어를 선정하고, 그 단어의 길이 만큼 칸을 만듦
//  a. 랜덤한 단어 rand() 선택 -> 선택한 단어 answer에 저장
//  b. 단어의 길이를 알아내서 그 값만큼 모든 반복을 지정
// [x]  c. 단어 칸 그리기는 길이만큼 반복하면서 특정 좌표만큼 옆으로 이동
//
// 2. 사용자에게 알파벳을 입력 받음
//  a. 선택된 단어와 사용자의 입력을 비교하기 위해 단어의 길이 만큼 반복
//  b. 알파벳에 해당 단어에 있다면 그 위치를 저장
// 2-1.
// [x]  a. 단어에 알파벳이 포함되어있지 않다면 칸의 배경을 어두운 색으로 바꿈
// [x]  b. 행맨의 한 부위가 매달림
// 2-2. 단어에 알파벳이 포함되어 있다면 알맞은 칸에 채워넣음
//  a. 알맞은 칸에 채워 넣는 것은 한 칸의 단어 상자(정답칸)을 그린 후에
//    위에서 저장한 위치를 보고 그려넣음
//    -> 길이만큼 int형 배열을 선언한 다음 bool 타입처럼 0, 1로 확인해서 결정하면 될 듯?

// 3-1. 행맨이 전부 매달리기 전에 단어를 다 맞춘다면 게임에서 승리
//  a. 단어 입력을 다 받은 후 위에 선언한 배열로 판단(전부 1이면 승리)
//  b. 승리하면 정답 단어를 다 출력하고 입력 칸에 게임 승리 출력 - 게임 다시 시작은 꼭 안해도 될 듯?
// 3-2. 5번의 기회를 다 소진했다면 게임에서 패배
//  a. WM_CHAR 마지막에 남은 기회 횟수를 판단

//입력칸 및 정답칸 그리기
// [x] 조건에 따라 행맨 그리기
//  행맨 그림을 전부 배열에 저장해놓고 하나씩 그리면 될듯?