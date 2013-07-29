#pragma once

#include "IFilterProcessor.h"
#include "BitmapUtils.h"

class NegativeFilterProcessor : public IFilterProcessor
{
public :
	 void ProcessImage(
		const Gdiplus::BitmapData& source,
		Gdiplus::BitmapData& dest
		)
	{
		int bpp = Gdiplus::GetPixelFormatSize(dest.PixelFormat);

		for (UINT x = 0; x < dest.Width; x++)
			for (UINT y = 0; y < dest.Height; y++)
			{
				COLORREF pixel = GetPixel(static_cast<byte *>(source.Scan0), x, y, dest.Stride, bpp);
				
				int newR = 255 - GetRValue(pixel);
				int newG = 255 - GetGValue(pixel);
				int newB = 255 - GetBValue(pixel);

				SetPixel(static_cast<byte *>(dest.Scan0), x, y, dest.Stride, bpp, RGB(newR, newG, newB));
			}
	}
};