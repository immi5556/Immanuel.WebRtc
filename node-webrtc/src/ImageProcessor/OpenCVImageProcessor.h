#if !defined(_H_OPENCVIMAGEPROCESSOR_) && defined(_OPENCV_PREBUILT_) && !defined(_IPP_)
#define _H_OPENCVIMAGEPROCESSOR_

class OpenCVImageProcessor : public ImageProcessor
{
private:
	static OpenCVImageProcessor* instance;

public:
	static OpenCVImageProcessor* getInstance(void);

public:
	OpenCVImageProcessor(void);
	OpenCVImageProcessor(OpenCVImageProcessor&) = delete;
	~OpenCVImageProcessor(void);

public:
	bool swap(const uint8_t* srcBuffer, const Area& srcSizem, uint8_t* dstBuffer, const Area& dstSize, const PixelType pixelType, const int* order) override;
	bool convert(PixelType srcPixelType, const uint8_t* srcBuffer, const Area& srcSize, PixelType dstPixelType, uint8_t* dstBuffer, const Area& dstSize) override;
	bool scale(PixelType pixelType, const uint8_t* srcBuffer, size_t srcBufferLength, const Area& srcSize, uint8_t* dstBuffer, size_t dstBufferLength, const Area& dstSize) override;
	bool value(PixelType pixelType, uint8_t* srcBuffer, size_t srcBufferLength, const Area& srcSize, int channelIndex, int value) override;
};

#endif
