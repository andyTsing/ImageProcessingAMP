#pragma once

#include <gdiplus.h>
#include <amp.h>

typedef unsigned long ArgbPackedPixel;

struct RgbPixel {
	unsigned int r;
	unsigned int g;
	unsigned int b;
};

inline ArgbPackedPixel PackPixel(const RgbPixel& rgb) restrict(amp)
{
	return rgb.b | (rgb.g << 8) | (rgb.r << 16) | (0xff << 24);
}

inline RgbPixel UnpackPixel(const ArgbPackedPixel& packedArgb) restrict(amp) 
{
    RgbPixel rgb;
    rgb.b = packedArgb & 0xFF;
    rgb.g = (packedArgb & 0xFF00) >> 8;
    rgb.r = (packedArgb & 0xFF0000) >> 16;
    return rgb;
}

void CopyIn(const Gdiplus::BitmapData& source, concurrency::array<ArgbPackedPixel, 2>& dest)
{
	ArgbPackedPixel* const start = static_cast<ArgbPackedPixel*>(source.Scan0);
	concurrency::copy(start, start + (source.Width * source.Height), dest);
}

void CopyOut(concurrency::array<ArgbPackedPixel, 2>& source, Gdiplus::BitmapData& dest)
{
    auto iter = stdext::make_checked_array_iterator<ArgbPackedPixel*>(static_cast<ArgbPackedPixel*>(dest.Scan0), dest.Height * dest.Width);

    concurrency::completion_future f = copy_async(source.section(0, 0, dest.Height, dest.Width), iter);
    source.accelerator_view.wait();
    f.get();
}