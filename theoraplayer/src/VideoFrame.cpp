/// @file
/// @version 2.0
/// 
/// @section LICENSE
/// 
/// This program is free software; you can redistribute it and/or modify it under
/// the terms of the BSD license: http://opensource.org/licenses/BSD-3-Clause

#include <memory.h>

#include "PixelTransform.h"
#include "Manager.h"

#include "VideoClip.h"
#include "VideoFrame.h"

//#define YUV_TEST // uncomment this if you want to benchmark YUV decoding functions

extern "C"
{
void decodeRGB  (struct PixelTransform* t);
void decodeRGBA (struct PixelTransform* t);
void decodeRGBX (struct PixelTransform* t);
void decodeARGB (struct PixelTransform* t);
void decodeXRGB (struct PixelTransform* t);
void decodeBGR  (struct PixelTransform* t);
void decodeBGRA (struct PixelTransform* t);
void decodeBGRX (struct PixelTransform* t);
void decodeABGR (struct PixelTransform* t);
void decodeXBGR (struct PixelTransform* t);
void decodeGrey (struct PixelTransform* t);
void decodeGrey3(struct PixelTransform* t);
void decodeGreyA(struct PixelTransform* t);
void decodeGreyX(struct PixelTransform* t);
void decodeAGrey(struct PixelTransform* t);
void decodeXGrey(struct PixelTransform* t);
void decodeYUV  (struct PixelTransform* t);
void decodeYUVA (struct PixelTransform* t);
void decodeYUVX (struct PixelTransform* t);
void decodeAYUV (struct PixelTransform* t);
void decodeXYUV (struct PixelTransform* t);
}

namespace theoraplayer
{
	static void(*conversion_functions[])(struct PixelTransform*) = { 0,
		decodeRGB,
		decodeRGBA,
		decodeRGBX,
		decodeARGB,
		decodeXRGB,
		decodeBGR,
		decodeBGRA,
		decodeBGRX,
		decodeABGR,
		decodeXBGR,
		decodeGrey,
		decodeGrey3,
		decodeGreyA,
		decodeGreyX,
		decodeAGrey,
		decodeXGrey,
		decodeYUV,
		decodeYUVA,
		decodeYUVX,
		decodeAYUV,
		decodeXYUV
	};

	VideoFrame::VideoFrame(VideoClip* parent)
	{
		this->ready = this->inUse = false;
		this->parent = parent;
		this->iteration = 0;
		// number of bytes based on output mode
		int bytemap[] = { 0, 3, 4, 4, 4, 4, 3, 4, 4, 4, 4, 1, 3, 4, 4, 4, 4, 3, 4, 4, 4, 4 };
		this->bpp = bytemap[this->parent->getOutputMode()];
		unsigned int size = this->parent->getStride() * this->parent->height * this->bpp;
		try
		{
			this->buffer = new unsigned char[size];
		}
		catch (std::bad_alloc)
		{
			this->buffer = NULL;
			return;
		}
		memset(this->buffer, 255, size);
	}

	VideoFrame::~VideoFrame()
	{
		if (this->buffer)
		{
			delete[] this->buffer;
		}
	}

	int VideoFrame::getWidth()
	{
		return this->parent->getWidth();
	}

	int VideoFrame::getStride()
	{
		return this->parent->stride;
	}

	int VideoFrame::getHeight()
	{
		return this->parent->getHeight();
	}

	unsigned char* VideoFrame::getBuffer()
	{
		return this->buffer;
	}

	void VideoFrame::decode(struct PixelTransform* t)
	{
		if (t->raw != NULL)
		{
			unsigned int bufferStride = this->parent->getWidth() * this->bpp;
			if (bufferStride == t->rawStride)
			{
				memcpy(this->buffer, t->raw, t->rawStride * this->parent->getHeight());
			}
			else
			{
				unsigned char *buff = this->buffer, *src = t->raw;
				int i, h = this->parent->getHeight();
				for (i = 0; i < h; ++i, buff += bufferStride, src += t->rawStride)
				{
					memcpy(buff, src, bufferStride);
				}
			}
		}
		else
		{
			t->out = this->buffer;
			t->w = this->parent->getWidth();
			t->h = this->parent->getHeight();

#ifdef YUV_TEST // when benchmarking yuv conversion functions during development, do a timed average
#define N 1000
			clock_t time = clock();
			for (int i = 0; i < N; ++i)
			{
				conversion_functions[mParent->getOutputMode()](t);
			}
			float diff = (clock() - time) * 1000.0f / CLOCKS_PER_SEC;

			char s[128];
			sprintf(s, "%.2f", diff / N);
			TheoraVideoManager::getSingleton().logMessage("YUV Decoding time: " + std::string(s) + " ms\n");
#else
			conversion_functions[this->parent->getOutputMode()](t);
#endif
		}
		this->ready = true;
	}

	void VideoFrame::clear()
	{
		this->inUse = this->ready = false;
	}

}