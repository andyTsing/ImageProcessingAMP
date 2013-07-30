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

		extent<2> e(h, w);
		const array<ArgbPackedPixel, 2> asource(e, static_cast<ArgbPackedPixel*>(source.Scan0));
		array<ArgbPackedPixel, 2> adest(e);
		//array<ArgbPackedPixel, 2> asource(e);
		//CopyIn(source, asource); 

		concurrency::parallel_for_each(asource.extent, [=, &asource, &adest](index<2> idx) restrict (amp) {
			RgbPixel accum;
			accum.r = 0; accum.g = 0; accum.b = 0;

			int count = 0;
			int x = idx[1];
			int y = idx[0];

			using namespace concurrency::direct3d;
			int startX = clamp(x - DIST, 0, w);
			int endX = clamp(x + DIST, 0, w);
			int startY = clamp(y - DIST, 0, h);
			int endY = clamp(y + DIST, 0, h);

			for (int currentX = startX ; currentX < endX; currentX++)
				for (int currentY = startY ; currentY < endY; currentY++)
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

