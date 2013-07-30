#pragma once

#include <amp.h>

#include "IFilterProcessor.h"
#include "BitmapUtils.h"
#include "AmpRgbPixel.h"

class BlurFilterProcessorAMP : public IFilterProcessor
{
public :
	 void ProcessImage(
		const Gdiplus::BitmapData& source,
		Gdiplus::BitmapData& dest
		)
	{
		using namespace concurrency;
		const int DIST = 10;
		const int w = source.Width;
		const int h = source.Height;

		array<ArgbPackedPixel, 2> asource(h, w), adest(h, w);

		CopyIn(source, asource); 

		concurrency::parallel_for_each(asource.extent, [=, &asource, &adest](index<2> idx) restrict (amp) {
			RgbPixel accum;
			accum.r = 0; accum.g = 0; accum.b = 0;

			int count = 0;
			int x = idx[1];
			int y = idx[0];
			for (int currentX = x - DIST ; currentX < (x + DIST); currentX++)
				if (currentX >= 0 && currentX < w)  
					for (int currentY = y - DIST ; currentY < (y + DIST); currentY++)
						if (currentY >= 0 && currentY < h) 
						{
							RgbPixel pixel = UnpackPixel(asource[index<2>(currentY, currentX)]);
							accum.r += pixel.r;
							accum.g += pixel.g;
							accum.b += pixel.b;
							count ++;
						}

			accum.r /= count; accum.g /= count; accum.b /= count;
			adest[idx] = PackPixel(accum);

		});

		CopyOut(adest, dest);
	}
};