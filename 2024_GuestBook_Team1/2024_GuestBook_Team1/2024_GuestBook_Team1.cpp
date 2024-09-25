﻿/**
@author 김형균
@date 2024.08.01
    기본 그리기 코드 추가
    그린 내용 유지 코드 추가
@todo 그린 내용 replay 구현
**/

#include "2024_GuestBook_Team1.h"
#include "Resource.h"


#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

HWND DrowBT = nullptr;
HWND LoadBT = nullptr;
HWND CreditBT = nullptr;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.


    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY2024GUESTBOOKTEAM1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY2024GUESTBOOKTEAM1));

    MSG msg;



    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//  
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_YUHAN256));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(243, 243, 243));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_YUHAN));

    WNDCLASSEXW nbcex = wcex;


    return RegisterClassExW(&wcex);
}

//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
static RECT DesktopRT;      //사용자 화면 크기 받기용
static RECT MainRT;         //메인 윈도우 크기 받기용
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    GetWindowRect(GetDesktopWindow(), &DesktopRT);

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        (DesktopRT.right - 1600) / 2, (DesktopRT.bottom - 900) / 2, 1600, 900, nullptr, nullptr, hInstance, nullptr);

    //제목 표시줄 색상 변경
    int aElements[] = { COLOR_ACTIVECAPTION };
    DWORD aOldColors[1];
    DWORD aNewColors[1];
    aOldColors[0] = GetSysColor(aElements[0]);
    aNewColors[0] = RGB(243, 243, 243);
    SetSysColors(1, aElements, aNewColors);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//


LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    string FileName = "제목 없음";
    static HWND SideMenu = NULL;
    static DrowWindow* dWindow = nullptr;
    static HWND d_hWnd = nullptr;
    static HWND b_hWnd = nullptr;

    switch (message)
    {
    case WM_CREATE:

        //fileManager->getInstance().InitializePanels(hWnd);

        GetClientRect(hWnd, &MainRT);


        DrowBT = CreateWindowW(L"BUTTON", L"서명하기", WS_CHILD | WS_VISIBLE,
            (MainRT.right / 2) - 120, (MainRT.bottom / 2) - 170, 240, 100, hWnd, (HMENU)DEF_DROW_BT, hInst, nullptr);
        LoadBT = CreateWindowW(L"BUTTON", L"불러오기", WS_CHILD | WS_VISIBLE,
            (MainRT.right / 2) - 120, (MainRT.bottom / 2) - 50, 240, 100, hWnd, (HMENU)DEF_LOAD_BT, hInst, nullptr);
        CreditBT = CreateWindowW(L"BUTTON", L"CREDIT", WS_CHILD | WS_VISIBLE,
            (MainRT.right / 2) - 120, (MainRT.bottom / 2) + 70, 240, 100, hWnd, (HMENU)DEF_CREDIT_BT, hInst, nullptr);

        dWindow = new DrowWindow(hInst);
        dWindow->Create(hWnd, 0, 0, MainRT.right, MainRT.bottom);
        dWindow->Show(FALSE);
        d_hWnd = dWindow->GetHWND();

        break;

    case WM_COMMAND:
    {

        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case DEF_DROW_BT:
            EnableWindow(d_hWnd, true);
            dWindow->ToolCnt = true;
            dWindow->Show(true);
            EnableWindow(DrowBT, FALSE);
            EnableWindow(LoadBT, FALSE);
            EnableWindow(CreditBT, FALSE);

            break;

        case DEF_LOAD_BT:
            EnableWindow(d_hWnd, true);
            dWindow->ToolCnt = FALSE;
            dWindow->Show(true);
            EnableWindow(DrowBT, FALSE);
            EnableWindow(LoadBT, FALSE);
            EnableWindow(CreditBT, FALSE);

            /*

            여기에다 파일 불러오는기능 추가해주세용

            */

            break;


        case DEF_CREDIT_BT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;

        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    }

    case WM_SIZE:
        //fileManager->getInstance().ResizePanels(hWnd, lParam);  /*패널 크기 조정 함수 호출*/
        GetClientRect(hWnd, &MainRT);

        MoveWindow(DrowBT, (MainRT.right / 2) - 120, (MainRT.bottom / 2) - 170, 240, 100, TRUE);
        MoveWindow(LoadBT, (MainRT.right / 2) - 120, (MainRT.bottom / 2) - 50, 240, 100, TRUE);
        MoveWindow(CreditBT, (MainRT.right / 2) - 120, (MainRT.bottom / 2) + 70, 240, 100, TRUE);

        MoveWindow(d_hWnd, 0, 0, MainRT.right, MainRT.bottom, TRUE);
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        HBRUSH hbr = (HBRUSH)SelectObject(hdc, CreateSolidBrush(RGB(249, 249, 249)));
        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
        GetClientRect(hWnd, &MainRT);

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        //function->GDIPlusEnd();
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;

    }
    return (INT_PTR)FALSE;
}