#include <Windows.h>
#include <gdiplus.h>

#include <iomanip>
#include <sstream>
#include <string>

#include "BitmapUtils.h"
#include "resource.h"

#include "IFilterProcessor.h"
#include "NegativeFilterProcessor.h"
#include "NegativeFilterProcessorAMP.h"
#include "BlurFilterProcessor.h"
#include "BlurFilterProcessorAMP.h"
#include "BlurFilterProcessorUsingTexturesAMP.h"

LRESULT CALLBACK WndProc(
	HWND hwnd,
	UINT message,
	WPARAM wparam,
	LPARAM lparam
	);

void OnPaint(HWND hwnd);
void OnCreate();
void OnDestroy();

BitmapPtr bitmap;

void OnFileOpenClick(HWND hwnd);
void OnFiltersNegativeClick();
void OnFiltersNegativeAMPClick();
void OnFiltersBlurClick();
void OnFiltersBlurAMPClick();
void OnFiltersBlurTextureAMPClick();

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PSTR szLineCmd,
	INT iCmdShow
	)
{
	WNDCLASS wndclass = {};
	wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndclass.lpszClassName = L"ImageEffectsAMP";
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
	wndclass.lpfnWndProc = WndProc;

	RegisterClass(&wndclass);

	HWND hwnd = CreateWindow(wndclass.lpszClassName, L"Image Processing AMP", 
		WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hwnd, iCmdShow);

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

void InvalidateWindow(HWND hwnd)
{
	RECT windowrect;
	GetClientRect(hwnd, &windowrect);
	InvalidateRect(hwnd, &windowrect, TRUE);
}


LRESULT CALLBACK WndProc(
	HWND hwnd,
	UINT message,
	WPARAM wparam,
	LPARAM lparam
	)
{
	switch (message)
	{
	case WM_PAINT:
		{
			OnPaint(hwnd);
			return 0;
		}
	case WM_COMMAND:
		{
			switch (LOWORD(wparam))
			{
			case IDC_FILE_EXIT: { DestroyWindow(hwnd); break; }
			case IDC_FILE_OPEN: { OnFileOpenClick(hwnd); break; }
			case IDC_FILTERS_NEGATIVE: { OnFiltersNegativeClick(); InvalidateWindow(hwnd); break; }
			case IDC_FILTERS_NEGATIVE_AMP: { OnFiltersNegativeAMPClick(); InvalidateWindow(hwnd); break;}
			case IDC_FILTERS_BLUR: { OnFiltersBlurClick(); InvalidateWindow(hwnd); break;}
			case IDC_FILTERS_BLUR_AMP: { OnFiltersBlurAMPClick(); InvalidateWindow(hwnd); break;}
			case IDC_FILTERS_BLUR_TEXTURES_AMP: { OnFiltersBlurTextureAMPClick(); InvalidateWindow(hwnd); break; }
			}
			return 0;
		}
	case WM_CREATE:
		{
			OnCreate();
			return 0;
		}
	case WM_DESTROY:
		{
			OnDestroy();
			PostQuitMessage(0);
			return 0;
		}

	default:
		return DefWindowProc(hwnd, message, wparam, lparam);
	}
}

void OnFileOpenClick(HWND hwnd)
{
	OPENFILENAME ofn;
	wchar_t szFile[1024];

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"JPEG\0*.jpg\0";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) == TRUE) {
		bitmap = LoadBitmapAndConvert(ofn.lpstrFile);
		InvalidateRect(hwnd, nullptr, TRUE);
	}
}

double ComputeElapsedTime(const LARGE_INTEGER& start, const LARGE_INTEGER& end)
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	return ((double(end.QuadPart) - double(start.QuadPart)) * 1000.0 / (double(frequency.QuadPart)));
}

double elapsedTime;
void ApplyFilter(IFilterProcessor& filter)
{
	LARGE_INTEGER start, end;
	QueryPerformanceCounter(&start);

	BitmapPtr inBitmap = bitmap;

	BitmapPtr outBitmap = BitmapPtr(
		inBitmap->Clone(0, 0, inBitmap->GetWidth(), inBitmap->GetHeight(),
		PixelFormat32bppARGB));

	Gdiplus::Rect rect(0, 0, inBitmap->GetWidth(), inBitmap->GetHeight());
	Gdiplus::BitmapData originalImage;
	inBitmap->LockBits(&rect, Gdiplus::ImageLockModeWrite,
		PixelFormat32bppARGB, &originalImage);

	Gdiplus::BitmapData processedImage;
	outBitmap->LockBits(&rect, Gdiplus::ImageLockModeWrite,
		PixelFormat32bppARGB, &processedImage);

	filter.ProcessImage(originalImage, processedImage);

	bitmap = outBitmap;

	inBitmap->UnlockBits(&originalImage);
	outBitmap->UnlockBits(&processedImage);

	QueryPerformanceCounter(&end);
	elapsedTime = ComputeElapsedTime(start, end);
}

void OnFiltersNegativeClick()
{
	NegativeFilterProcessor negative;
	ApplyFilter(negative);
}

void OnFiltersNegativeAMPClick()
{
	NegativeFilterProcessorAMP negativeAMP;
	ApplyFilter(negativeAMP);
}

void OnFiltersBlurClick()
{
	BlurFilterProcessor blur;
	ApplyFilter(blur);
}

void OnFiltersBlurAMPClick()
{
	BlurFilterProcessorAMP blurAMP;
	ApplyFilter(blurAMP);
}

void OnFiltersBlurTextureAMPClick()
{
	BlurFilterProcessorUsingTexturesAMP blurAMP;
	ApplyFilter(blurAMP);
}
// --------------------------------------

ULONG_PTR gdiPlusToken;
void OnCreate()
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInit;
	Gdiplus::GdiplusStartup(&gdiPlusToken, &gdiplusStartupInit, nullptr);
}

void OnDestroy()
{
	if (bitmap) bitmap.reset();
	Gdiplus::GdiplusShutdown(gdiPlusToken);
}

void OnPaint(HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);

	if (bitmap)
	{
		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP hb;
		BITMAP bm;
		bitmap->GetHBITMAP(Gdiplus::Color(0,0,0), &hb);
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hb);
		GetObject(hb, sizeof(bm), &bm);

		RECT rect;
		GetClientRect(hwnd, &rect);
		FillRect(hdc, &rect, (HBRUSH) GetStockObject(WHITE_BRUSH));

		//StretchBlt(hdc, client.left, client.top, client.right - client.left, client.bottom - client.top, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
		BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
		
		SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem);
		DeleteObject(hb);

		std::wstringstream stream;
		stream << std::fixed << std::setprecision(2) << elapsedTime;
		std::wstring s = stream.str();
		TextOut(hdc, 0, 0, s.c_str(), s.length());
	}

	EndPaint(hwnd, &ps);
}