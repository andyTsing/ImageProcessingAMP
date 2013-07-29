#pragma once

#include "IFilterProcessor.h"
#include "BitmapUtils.h"

class BlurFilterProcessor : public IFilterProcessor
{
public :
	 void ProcessImage(
		const Gdiplus::BitmapData& source,
		Gdiplus::BitmapData& dest
		)
	{
		int bpp = Gdiplus::GetPixelFormatSize(dest.PixelFormat);
		int dist = 5;

		int w = source.Width;
		int h = source.Height;

		for (int x = 0; x < dest.Width; x++)
			for (int y = 0; y < dest.Height; y++)
			{
				int accumR = 0;
				int accumG = 0;
				int accumB = 0;
				int count = 0;
				for (int currentX = x - dist ; currentX < (x + dist); currentX++)
					if (currentX >= 0 && currentX < w)  
						for (int currentY = y - dist ; currentY < (y + dist); currentY++)
							if (currentY >= 0 && currentY < h) 
							{
								COLORREF pixel = GetPixel(static_cast<byte *>(source.Scan0), currentX, currentY, dest.Stride, bpp);
								accumR += GetRValue(pixel);
								accumG += GetGValue(pixel);
								accumB += GetBValue(pixel);
								count ++;
							}

				SetPixel(static_cast<byte *>(dest.Scan0), x, y, dest.Stride, bpp, RGB(accumR / count, accumG / count, accumB / count));
			}
	}
};