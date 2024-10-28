#include "DW_Canvas.h"


DW_Canvas::DW_Canvas(HINSTANCE hInstance)
    :ChildWindow(RGB(255, 255, 255))
{
    cInst = hInstance;
    canvasRT = { 0 };
    canWnd = nullptr;
}

void DW_Canvas::Create(HWND hParentWnd, int x, int y, int width, int height)
{
    ChildWindow::Create(hParentWnd, L"DW_CanvasClass", L"Canvas Child Window", x, y, width, height);
    canWnd = cWnd;

    Function::hWnd = canWnd;
    penThickness = make_unique<PenThickness>();
}


LRESULT DW_Canvas::HandleMessage(HWND cWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message)
    {
    case WM_CREATE:
        function = make_unique<Function>();
        function->GDIPlusStart(); // º× gdi ¶óÀÌºê·¯¸® È°¼ºÈ­
        break;

    case WM_COMMAND:
        if (wParam == TL_CLEAR_BT)
        {
            if (function->getIsReset())
            {
                function->clearDrawing(cWnd);
            }

        }

        if (wParam == TL_PLAY_BT && lParam == 0)
        {
            if (!function->getIsReplay())
            {  
                function->replayThread(cWnd);
            }
            else
            {
                function->resumeReplay();
            }
        }
        

        if (wParam == TL_PLAY_BT && lParam == 1)
        {
            function->suspendReplay();
        }
        
        if (wParam == TL_RESET_BT)
        {
            function->reDrawing(cWnd);
        }

        if (LOWORD(wParam) == 1) 
        {
        }
        break;

    case WM_MOUSEMOVE:
        if (!function->getIsReset()) break;
        hdc = GetDC(canWnd);

        drawPInfo.lParam = lParam;
        drawPInfo.pColor = ColorPalette::colorArr[Function::penNum];
        drawPInfo.pTime = (DWORD)GetTickCount64();
        //drawPInfo.pWidth = penThickness->getPenWidth(); /// Ææ ±½±â ¼³Á¤
        drawPInfo.pWidth = 8; /// Ææ ±½±â ¼³Á¤
        drawPInfo.state = message;
        function->draw(cWnd, drawPInfo, TRUE); // ºê·¯½¬ ±â´É Ãß°¡ÇÏ·Á¸é ÇØ´ç RECTANGLE ¿¡ ¾Ë¸Â´Â º¯¼ö¸¦ ³ÖÀ¸¸é µÊ.
        break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
        if (!function->getIsReset()) break;
        drawPInfo.lParam = lParam;
        drawPInfo.pColor = ColorPalette::colorArr[Function::penNum];
        drawPInfo.pTime = (DWORD)GetTickCount64();
        //drawPInfo.pWidth = penThickness->getPenWidth(); /// Ææ ±½±â ¼³Á¤
        drawPInfo.pWidth = 8; /// Ææ ±½±â ¼³Á¤
        drawPInfo.state = message;
        function->mouseUD(drawPInfo, TRUE);

        break;

    case WM_RBUTTONDOWN:

        break;

    case WM_SIZE:


    case WM_ERASEBKGND:
        ///È­¸é ±ôºýÀÓ ¹æÁö
        return DefWindowProc(cWnd, message, wParam, lParam);
        break;
    case WM_PAINT:
        canvasRT = ChildWindow::GetRT();
        function->paint(canWnd, canvasRT);

    default:
        return ChildWindow::HandleMessage(canWnd, message, wParam, lParam);
    }
    return 0;
}