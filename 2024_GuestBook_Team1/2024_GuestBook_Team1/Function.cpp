/**
@author ������
@date 2024.10.28
	----------���� �� �� --------------
	�������� ��� ���� - �⺻ũ�� Ű���, ũ�⿡ ���� �������� �ѷ����� �е� ����
	���� ��� ���� - ���� ũ�� ����
	�� �귯�� ��� ���� - ���⿡ ���� ������ �׸� ���� ��������, �� �귯�� ���� �ּ� ���� ������.
	----------���� �ؾ� �Ұ� -----------
	â ���� �� ���� �Ÿ��°�
	â ���� �� �� �귯�� ��� ������°�
**/
#include "Function.h"


using namespace std;


int Function::penNum = 0;
LINFO Function::drawLInfo = { };
HWND Function::hWnd = nullptr;
int Function::bShape = BASIC;

void Function::record(PINFO inputPI)
{
	//if (isReplay) return;

	inputPI.bShape = bShape;
	inputPI.pWidth = currentThickness;
	drawLInfo.pInfo.push_back(inputPI);

	/*std::wstring message = L"record() ȣ���, drawLInfo.pInfo ũ��: " + std::to_wstring(drawLInfo.pInfo.size()) +
		L"\nFunction ��ü �ּ�: " + std::to_wstring((uintptr_t)this);
	MessageBox(nullptr, message.c_str(), L"�����: record", MB_OK); */



}

void Function::draw(HWND hWnd, PINFO dInfo, bool isRecord) // �ڿ� �귯�� �߰�
{

	hdc = GetDC(hWnd);
	if (isLeftClick)
	{
		px = LOWORD(dInfo.lParam); // �׸��� ������ ��ǥ
		py = HIWORD(dInfo.lParam);			
		
		currentTime = std::chrono::steady_clock::now(); // �׸��� �ð� ����

		setPenStyle(dInfo, dInfo.pColor);

		MoveToEx(hdc, x, y, NULL);
		LineTo(hdc, px, py);
		binfo DRAW;
		DRAW.current = currentThickness;
		BINFO.push_back(DRAW);

		SelectObject(hdc,oPen); //��ü ����
		DeleteObject(nPen); //��ü ����

		x = px;
		y = py;

		DrawTime = currentTime; // ������ �ð� ������Ʈ

		if (isRecord)
			record(dInfo);

	}
	ReleaseDC(hWnd, hdc);

}

void Function::mouseUD(PINFO dInfo, bool isRecord)
{
	if (dInfo.state == WM_LBUTTONDOWN)
	{
		x = LOWORD(dInfo.lParam); //��Ŭ�� �� ��ǥ
		y = HIWORD(dInfo.lParam);

		DrawTime = std::chrono::steady_clock::now(); // �� �귯�� ����� ���� �׸��� ���� �ð� ����.		
		lastThicknessChangeTime = DrawTime; // �β� ���� �ð� �ʱ�ȭ
		currentThickness = dInfo.pWidth; // �� ���� �� ������.


		isLeftClick = true;
	}
	else
	{
		isLeftClick = false;
	}


	if (isRecord)
		record(dInfo);

}

void Function::replayThread(HWND hWnd)
{
	setIsReplay(true);
	setIsReset(false);

	// std::thread�� ����Ͽ� ������ ����
	replayThreadHandle = thread(&Function::replay, this, hWnd);

	threadHandle = replayThreadHandle.native_handle();
}


 //�⺻ ���÷��� ���� �Լ�
void Function::replay(HWND hWnd)
{
	// ȭ�� �ʱ�ȭ
	HDC hdc, memDC;
	HBITMAP hBitmap;
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);  // Ŭ���̾�Ʈ ���� ũ�� ���

	while (isReplay)
	{
		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);

		// ȭ�� DC ��������
		hdc = GetDC(hWnd);

		// �޸� DC ���� �� ȣȯ ��Ʈ�� �Ҵ�
		memDC = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
		SelectObject(memDC, hBitmap);

		// �޸� DC���� ��� �����
		FillRect(memDC, &clientRect, (HBRUSH)(COLOR_WINDOW + 1));

		// �׸��� �۾� �޸� DC���� ����
		for (size_t i = 0; i < drawLInfo.pInfo.size(); i++)
		{
			if (!isReplay)
			{
				isLeftClick = false;
				break;
			}
			else
				isLeftClick = true;

			PINFO replayInfo = drawLInfo.pInfo[i];

			setBShape(replayInfo.bShape);

			switch (replayInfo.state)
			{
			case WM_LBUTTONDOWN:
				mouseUD(replayInfo, false);
				break;

			case WM_MOUSEMOVE:
				draw(hWnd, replayInfo, false);
				break;

			case WM_LBUTTONUP:
				mouseUD(replayInfo, false);
				break;
				
			default:
				break;
			}

			// ��� �ӵ� ����
			if (i + 1 < drawLInfo.pInfo.size() && drawLInfo.pInfo[i + 1].state == WM_MOUSEMOVE)
			{
				Sleep((int)((drawLInfo.pInfo[i + 1].pTime - drawLInfo.pInfo[i].pTime) / 10));
			}

			DeleteObject(nPen);
		}

		// �޸� DC�� ������ ���� ȭ�鿡 ����
		BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, memDC, 0, 0, SRCCOPY);

		// �޸� DC�� ��Ʈ�� ����
		DeleteObject(hBitmap);
		DeleteDC(memDC);

		ReleaseDC(hWnd, hdc);

		// �ݺ� ���� ����
		Sleep(500);
	}
}


// RESET ��ư Ŭ�� �� �۵��Ǵ� �Լ� (���� ���·� ����)
void Function::reDrawing(HWND hWnd)
{
	if (replayThreadHandle.joinable())
	{
		isReplay = false;
		ResumeThread(threadHandle);
		stopReplay(hWnd);
	}

	InvalidateRect(hWnd, NULL, TRUE);
	UpdateWindow(hWnd);

	
}

void Function::clearDrawing(HWND hWnd)
{
	if (replayThreadHandle.joinable())
	{
		isReplay = false;
		ResumeThread(threadHandle);
		stopReplay(hWnd);
	}

	// ��� ����
	drawLInfo.pInfo.clear();

	// ȭ�� �ʱ�ȭ
	InvalidateRect(hWnd, NULL, TRUE);
	UpdateWindow(hWnd);	
}

void Function::setPenStyle(PINFO dinfo, COLORREF col)
{
	// �귯�� �����ϸ� �ű⿡ �´� �� ����
	switch (bShape)
	{
	case BASIC: // �⺻ �׸���
		nPen = CreatePen(PS_SOLID, dinfo.pWidth, col);
		oPen = (HPEN)SelectObject(hdc, nPen);
		break;

	case BRUSH: // �� �귯��
	{
		if (!isReplay || isReset)
		{
			int pwidth = dinfo.pWidth;
			if (dinfo.pWidth < 8) { pwidth = 7; }
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - DrawTime).count(); //������ �� �ð����� �׷��� �� �ð� �� ������ �ð�(�и���)
			duration = max(duration, 1); // �ּ� duration ���� �����Ͽ� 0���� ������ ���� ����

			double distance = sqrt(pow(px - x, 2) + pow(py - y, 2)); // �� �Ÿ�
			double speed = (distance / duration) * 1000; // �ӵ� ���

			int targetThickness = pwidth; // �ӵ��� ����� �� ���� ���� �Ǵ� �β� ����

			// �ӵ��� ���� �� �β� ���̱�
			if (speed > Threshold_Speed) {
				targetThickness = pwidth - (int)((speed - Threshold_Speed) / (Threshold_Speed / (pwidth - Min_Thickness +1)));
				targetThickness = max(targetThickness, Min_Thickness);
			}
			// �ӵ��� ���� �� �β� �ø���
			else {
				targetThickness = Min_Thickness + (int)((Threshold_Speed - speed) / (Threshold_Speed / (pwidth - Min_Thickness +1)));
				targetThickness = min(targetThickness, pwidth);
			}

			// �β� ��ȭ ������ �������� Ȯ��		
			if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastThicknessChangeTime).count() >= Update_Interval)
			{
				// �� ���� ������ ����
				if (currentThickness < targetThickness)
					currentThickness += Smoothing_Factor;
				else if (currentThickness > targetThickness)
					currentThickness -= Smoothing_Factor;

				// �β� ���� �ð� ������Ʈ
				lastThicknessChangeTime = currentTime;
			}
			// �β��� ����� �� ����
			nPen = CreatePen(PS_SOLID, currentThickness, col);
		}
		else  // ���÷��� �߿��� �β��� ����
		{
			nPen = CreatePen(PS_SOLID, dinfo.pWidth, col);  // �׸� �� ����� �β� ���
		}
		oPen = (HPEN)SelectObject(hdc, nPen);
		break;
	}

	case PENCIL:
	{
		Gdiplus::Graphics graphics(hdc);
		int alpha = 45; // �⺻ ���� ����    		
		Gdiplus::PointF points[80]; // ���� ������ ����
		for (int i = 0; i < 80; ++i)
		{
			INT angle = rand() % 6 * 3.14159f * i / 80; // ������ ��ǥ 
			points[i] = Gdiplus::PointF(x + dinfo.pWidth * cos(angle) * 1.2 , y + dinfo.pWidth * sin(angle) * 1.2); // ������ ����
		}
		Gdiplus::SolidBrush brush(Gdiplus::Color(alpha, GetRValue(col), GetGValue(col), GetBValue(col)));	// ���� ����
		graphics.FillPolygon(&brush, points, 80); // ����ȭ ���� �ʴ� ���� �׸���	
		ReleaseDC(hWnd, hdc);
		break;
	}

	case SPRAY: // �������� (���� ��Ѹ�)
		if (dinfo.pWidth <= 3) { spray_pixel = 70; } // �� ���⿡ ���� �� �Ѹ��� �е�
		else if (dinfo.pWidth >= 4 && dinfo.pWidth <=6) { spray_pixel = 180; }
		else if (dinfo.pWidth >= 7 && dinfo.pWidth <= 10) { spray_pixel = 290; }
		else if (dinfo.pWidth >= 11 && dinfo.pWidth <= 14) { spray_pixel = 400; }
		else if (dinfo.pWidth >= 15 && dinfo.pWidth <= 20) { spray_pixel = 510; }
		for ( int i=0; i < spray_pixel; ++i)
		{
			int offsetX = (rand() % (dinfo.pWidth * 8)) - (dinfo.pWidth * 4);
			int offsetY = (rand() % (dinfo.pWidth * 8)) - (dinfo.pWidth * 4);
			if (sqrt(offsetX * offsetX + offsetY * offsetY) <= dinfo.pWidth * 4)
			{
				SetPixel(hdc, x + offsetX, y + offsetY, col);
			}
		}
		ReleaseDC(hWnd, hdc);
		break;

	case MARKER:
	{
		Gdiplus::Graphics graphics(hdc);
		Gdiplus::SolidBrush marker(Gdiplus::Color(40, GetRValue(col), GetGValue(col), GetBValue(col)));
		graphics.FillRectangle(&marker, x - dinfo.pWidth, y - dinfo.pWidth, dinfo.pWidth * 2, dinfo.pWidth * 2);
		ReleaseDC(hWnd, hdc);
		break;
	}

	case WATERCOLOR:
	{
		Gdiplus::Graphics graphics(hdc);
		int alpha = 10; // �⺻ ���� ����
		const int numPoints = 20; // ������ ����
		Gdiplus::PointF points[numPoints];

		// ������ ������ ����Ͽ� ���������� ����� �����
		for (int i = 0; i < numPoints; ++i) {
			float angle = 2 * 3.14159f * i / numPoints; // ���� ��ǥ
			float radius = dinfo.pWidth + (rand() % 10); // ������ �ݰ� ��ȭ
			points[i] = Gdiplus::PointF(x + radius * cos(angle), y + radius * sin(angle)); // ������ ����
		}

		Gdiplus::SolidBrush brush(Gdiplus::Color(alpha, GetRValue(col), GetGValue(col), GetBValue(col))); // ���� ����
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias); // �ε巴�� �׸���
		graphics.FillPolygon(&brush, points, numPoints); // ������ ���� �׸���

		ReleaseDC(hWnd, hdc);
		break;
	}

	default:
		break;
	}
}

void Function::paint(HWND hWnd, RECT canvasRT)
{
	cHdc = BeginPaint(hWnd, &cPS);
	
	if (!getIsReplay())
	{		
		if(this->bShape==BRUSH){			
		for (size_t i = 1; i < BINFO.size(); ++i) {			
			HPEN bpen = CreatePen(PS_SOLID, BINFO[i].current, RGB(0, 0, 0));
			HPEN bbpen = (HPEN)SelectObject(hdc, bpen);			
			}
		}

		
		
		for (const auto& record : getDrawLInfo().pInfo)
		{
			if (record.bShape != BRUSH)
				setBShape(record.bShape);
			else setBShape(BASIC);

			switch (record.state)
			{
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
				mouseUD(record, FALSE);
				break;

			case WM_MOUSEMOVE:
				draw(hWnd, record, FALSE);
				break;

			

			default:
				break;
			}
			
		}
	
	}

	EndPaint(hWnd, &cPS);
}




void Function::setBShape(int bShape)
{
	this->bShape = bShape;
}

LINFO Function::getDrawLInfo()
{
	/*std::wstring message = L"getDrawLInfo() ȣ��, drawLInfo.pInfo ũ��: " + std::to_wstring(drawLInfo.pInfo.size()) +
		L"\nFunction ��ü �ּ�: " + std::to_wstring((uintptr_t)this);
	MessageBox(nullptr, message.c_str(), L"�����: getDrawLInfo", MB_OK); */
	return drawLInfo;
}

void Function::setIsReplay(bool isReplay)
{
	this->isReplay = isReplay;
}

bool Function::getIsReplay()
{
	return isReplay;
}

void Function::setIsReset(bool isReset)
{
	this->isReset = isReset;
}

bool Function::getIsReset()
{
	return isReset;
}

void Function::suspendReplay()

{
	setIsReplay(true);
	setIsReset(true);
	isLeftClick = false;
	SuspendThread(threadHandle);
	px2 = px;
	py2 = py;
}

void Function::resumeReplay()
{	
	setIsReset(false);
	setIsReplay(true);
	ResumeThread(threadHandle);
	isLeftClick = true;
}

void Function::stopReplay(HWND hWnd)
{
	setIsReplay(false);
	setIsReset(true);

	if (replayThreadHandle.joinable())
	{
		replayThreadHandle.join();
	}
}



// ���Ͱ� ����ִ��� �˻�
bool Function::getDrawLInfoEmpty()
{
	return drawLInfo.pInfo.empty();
}
void Function::GDIPlusStart()
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}
void Function::GDIPlusEnd() { //gdi ����
	Gdiplus::GdiplusShutdown(gdiplusToken);
}
