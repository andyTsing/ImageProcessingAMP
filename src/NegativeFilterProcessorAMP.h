#pragma once

#include <amp.h>

#include "IFilterProcessor.h"
#include "BitmapUtils.h"
#include "AmpRgbPixel.h"

class NegativeFilterProcessorAMP : public IFilterProcessor
{
public :
	 void ProcessImage(
		const Gdiplus::BitmapData& source,
		Gdiplus::BitmapData& dest
		)
	{
		using namespace concurrency;

		array<ArgbPackedPixel, 2> a((int)source.Height, (int)source.Width);

		CopyIn(source, a); 

		concurrency::parallel_for_each(a.extent, [=, &a](index<2> idx) restrict (amp) {
			RgbPixel pixel = UnpackPixel(a[idx]);
			pixel.r = 255 - pixel.r;
			pixel.g = 255 - pixel.g;
			pixel.b = 255 - pixel.b;
			a[idx] = PackPixel(pixel);
		});

		CopyOut(a, dest);
	}
};