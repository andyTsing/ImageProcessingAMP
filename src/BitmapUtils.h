#pragma once

#include <gdiplus.h>
#include <memory>
#include <string>

typedef std::shared_ptr<Gdiplus::Bitmap> BitmapPtr;

 static BitmapPtr LoadBitmapAndConvert(const std::wstring& filePath)
{
    std::unique_ptr<Gdiplus::Bitmap> temp = std::unique_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(filePath.c_str()));
    return BitmapPtr(temp->Clone(0, 0, temp->GetWidth(), temp->GetHeight(), PixelFormat32bppARGB));
}

 static inline COLORREF GetPixel(const byte* const pFrame, int x, int y, int pitch, int bpp)
{
    const int width = abs(static_cast<int>(pitch));
    const int bytesPerPixel = bpp / 8;
    const int byteIndex = y * width + x * bytesPerPixel;
    return RGB(pFrame[byteIndex + 2], pFrame[byteIndex + 1], pFrame[byteIndex]);
}

static inline void SetPixel(byte* const pFrame, int x, int y, int pitch, int bpp, COLORREF color)
{
    const int width = abs(static_cast<int>(pitch));
    const int bytesPerPixel = bpp / 8;
    const int byteIndex = y * width + x * bytesPerPixel;
    pFrame[byteIndex + 2] = GetRValue(color);
    pFrame[byteIndex + 1] = GetGValue(color);
    pFrame[byteIndex] = GetBValue(color);
}