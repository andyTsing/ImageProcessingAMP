#pragma once

#include <gdiplus.h>

class IFilterProcessor
{
public: 
	virtual void ProcessImage(
		const Gdiplus::BitmapData& source,
		const Gdiplus::BitmapData& dest
		) = 0;
};

