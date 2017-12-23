#ifndef _H_IMAGEPROCESSOR_
#define _H_IMAGEPROCESSOR_

enum PixelType
{
	/* 3 pixel channel */
	RGB24 = 1,
	BGR24 = 2,

	/* 4 pixel channel */
	ARGB32 = 1024,
	RGBA32 = 2048,
};

class ImageProcessor
{
public:
	ImageProcessor(void);
	virtual ~ImageProcessor(void) = 0;

private:
	ImageProcessor(ImageProcessor&) = delete;

public:
	virtual bool swap(const uint8_t* srcBuffer, const Area& srcSize, uint8_t* dstBuffer, const Area& dstSize, const PixelType pixelType, const int* order) = 0;
	virtual bool convert(PixelType srcPixelType, const uint8_t* srcBuffer, const Area& srcSize, PixelType dstPixelType, uint8_t* dstBuffer, const Area& dstSize) = 0;
	virtual bool scale(PixelType pixelType, const uint8_t* srcBuffer, size_t srcBufferLength, const Area& srcSize, uint8_t* dstBuffer, size_t dstBufferLength, const Area& dstSize) = 0;
	virtual bool value(PixelType pixelType, uint8_t* srcBuffer, size_t srcBufferLength, const Area& srcSize, int channelIndex, int value) = 0;
};

#endif