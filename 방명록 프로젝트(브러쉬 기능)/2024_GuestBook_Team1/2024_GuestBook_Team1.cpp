#include <windows.h>
#include <vector>
#include <cmath>

// 전역 변수
COLORREF currentColor = RGB(0, 0, 0); // 기본 색상: 검정
int brushSize = 5; // 기본 브러시 크기
bool isDrawing = false;
POINT lastPoint;
enum BrushType { BASIC, BRUSH, PEN, DASHED, THICK, BALLPEN, PENCIL, SPRAY, MARKER, AIRBRUSH };

BrushType currentBrush = BASIC;

// 함수 선언
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void DrawLine(HDC hdc, POINT start, POINT end);
void ChooseColor(HWND hwnd);
void SetBrushSize(HWND hwnd);
void SetBrushType(HWND hwnd, BrushType brushType);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"DrawingWindowClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Simple Drawing Program",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HDC hdc;
    static PAINTSTRUCT ps;

    switch (uMsg) {
    case WM_CREATE: {
        HMENU hMenu = CreateMenu();
        AppendMenu(hMenu, MF_STRING, 1, L"Choose Color");
        AppendMenu(hMenu, MF_STRING, 2, L"Set Brush Size");
        AppendMenu(hMenu, MF_STRING, 3, L"Basic Brush");
        AppendMenu(hMenu, MF_STRING, 4, L"Brush");
        AppendMenu(hMenu, MF_STRING, 5, L"Pen");
        AppendMenu(hMenu, MF_STRING, 6, L"Dashed Brush");
        AppendMenu(hMenu, MF_STRING, 7, L"Thick Brush");
        AppendMenu(hMenu, MF_STRING, 8, L"Ball Pen");  // 볼펜
        AppendMenu(hMenu, MF_STRING, 9, L"Pencil");   // 연필
        AppendMenu(hMenu, MF_STRING, 10, L"Spray");   // 스프레이
        AppendMenu(hMenu, MF_STRING, 11, L"Marker");
        AppendMenu(hMenu, MF_STRING, 12, L"Airbrush");
        SetMenu(hwnd, hMenu);
        break;
    }



    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case 1:
            ChooseColor(hwnd);
            break;
        case 2:
            SetBrushSize(hwnd);
            break;
        case 3:
            SetBrushType(hwnd, BASIC);
            break;
        case 4:
            SetBrushType(hwnd, BRUSH);
            break;
        case 5:
            SetBrushType(hwnd, PEN);
            break;
        case 6:
            SetBrushType(hwnd, DASHED);
            break;
        case 7:
            SetBrushType(hwnd, THICK);
            break;
        case 8:
            SetBrushType(hwnd, BALLPEN);  // 볼펜 선택
            break;
        case 9:
            SetBrushType(hwnd, PENCIL);   // 연필 선택
            break;
        case 10:
            SetBrushType(hwnd, SPRAY);    // 스프레이 선택
            break;
        case 11:
            SetBrushType(hwnd, MARKER);
            break;
        case 12:
            SetBrushType(hwnd, AIRBRUSH);
            break;
        }
        break;
    }



    case WM_LBUTTONDOWN: {
        isDrawing = true;
        lastPoint.x = LOWORD(lParam);
        lastPoint.y = HIWORD(lParam);
        break;
    }

    case WM_LBUTTONUP: {
        isDrawing = false;
        break;
    }

    case WM_MOUSEMOVE: {
        if (isDrawing) {
            POINT currentPoint;
            currentPoint.x = LOWORD(lParam);
            currentPoint.y = HIWORD(lParam);

            hdc = GetDC(hwnd);
            DrawLine(hdc, lastPoint, currentPoint);
            ReleaseDC(hwnd, hdc);

            lastPoint = currentPoint;
        }
        break;
    }

    case WM_PAINT: {
        hdc = BeginPaint(hwnd, &ps);
        EndPaint(hwnd, &ps);
        break;
    }

    case WM_DESTROY: {
        PostQuitMessage(0);
        break;
    }

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void DrawLine(HDC hdc, POINT start, POINT end) {
    HBRUSH hBrush;
    HPEN hPen;
    int penSize = brushSize;

    switch (currentBrush) {
    case BASIC:
        hPen = CreatePen(PS_SOLID, brushSize, currentColor);
        break;
    case BRUSH:
        hPen = CreatePen(PS_SOLID, brushSize * 3, currentColor); // 더 두꺼운 붓 효과
        break;
    case PEN:
        hPen = CreatePen(PS_DASH, brushSize, currentColor);  // 점선
        break;
    case DASHED:
        hPen = CreatePen(PS_DASH, brushSize, currentColor);
        break;
    case THICK:
        hPen = CreatePen(PS_SOLID, brushSize * 2, currentColor);
        break;
    case BALLPEN:
        hPen = CreatePen(PS_SOLID, brushSize, currentColor);
        break;
    case PENCIL:
        hPen = CreatePen(PS_SOLID, brushSize, RGB(105, 105, 105));  // 연필: 연한 회색
        break;
    case SPRAY: {
        // 스프레이 효과: 원형 점을 랜덤하게 흩뿌림
        for (int i = 0; i < 100; ++i) {
            int offsetX = (rand() % (brushSize * 2)) - brushSize;
            int offsetY = (rand() % (brushSize * 2)) - brushSize;
            SetPixel(hdc, start.x + offsetX, start.y + offsetY, currentColor);
        }
    case MARKER:
        // 마커 효과: 반투명한 브러시로 두꺼운 선
        hBrush = CreateSolidBrush(RGB(GetRValue(currentColor), GetGValue(currentColor), GetBValue(currentColor)));
        SelectObject(hdc, hBrush);
        Rectangle(hdc, start.x - penSize, start.y - penSize, start.x + penSize, start.y + penSize);
        DeleteObject(hBrush);
        return;
    case AIRBRUSH:
        // 에어브러시 효과: 점들을 부드럽게 흩뿌림
        for (int i = 0; i < 100; ++i) {
            int offsetX = (rand() % (penSize * 8)) - (penSize * 4);
            int offsetY = (rand() % (penSize * 8)) - (penSize * 4);
            if (sqrt(offsetX * offsetX + offsetY * offsetY) <= penSize * 4) {
                SetPixel(hdc, start.x + offsetX, start.y + offsetY, currentColor);
            }
        }
        return;
    }
    default:
        hPen = CreatePen(PS_SOLID, brushSize, currentColor);
        break;
    }

    SelectObject(hdc, hPen);
    MoveToEx(hdc, start.x, start.y, NULL);
    LineTo(hdc, end.x, end.y);
    DeleteObject(hPen);
}

void ChooseColor(HWND hwnd) {
    CHOOSECOLOR cc;
    static COLORREF acrCustClr[16];
    ZeroMemory(&cc, sizeof(cc));
    cc.lStructSize = sizeof(cc);
    cc.hwndOwner = hwnd;
    cc.lpCustColors = acrCustClr;
    cc.rgbResult = currentColor;
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;

    if (ChooseColor(&cc)) {
        currentColor = cc.rgbResult;
    }
}

void SetBrushSize(HWND hwnd) {
    // 브러시 크기를 설정할 수 있는 간단한 다이얼로그 예제
    brushSize = 3; // 예시로 설정
}

void SetBrushType(HWND hwnd, BrushType brushType) {
    currentBrush = brushType;
}
