#pragma once

#include <amp.h>
#include <amp_graphics.h>

#include "IFilterProcessor.h"
#include "BitmapUtils.h"

class BlurFilterProcessorUsingTexturesAMP : public IFilterProcessor
{
public :
	void ProcessImage(
		const Gdiplus::BitmapData& source,
		Gdiplus::BitmapData& dest
		)
	{
		using namespace concurrency;
		using namespace concurrency::graphics;
		using namespace concurrency::direct3d;

		const int DIST = 10;
		const int w = source.Width;
		const int h = source.Height;

		extent<2> e(h, w);
		texture<uint_4, 2> asource(e, 8u);
		copy(source.Scan0, source.Stride * source.Height, asource);

		texture<uint_4, 2> adest_texture(e, 8u);
		writeonly_texture_view<uint_4, 2> adest(adest_texture);

		concurrency::parallel_for_each(asource.extent, [=, &asource](index<2> idx) restrict (amp) {
			uint_4 accum;

			int count = 0;
			int x = idx[1];
			int y = idx[0];

			int startX = clamp(x - DIST, 0, w);
			int endX = clamp(x + DIST, 0, w);
			int startY = clamp(y - DIST, 0, h);
			int endY = clamp(y + DIST, 0, h);

			for (int currentX = startX ; currentX < endX; currentX++)
			{
				for (int currentY = startY ; currentY < endY; currentY++)
				{
					accum += asource.get(index<2>(currentY, currentX));
					count ++;
				}
			}

			accum /= count;

			adest.set(idx, accum);
		});

		adest.accelerator_view.wait();

		copy(adest_texture, dest.Scan0, source.Stride * source.Height);
	}
};

