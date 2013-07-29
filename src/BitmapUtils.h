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