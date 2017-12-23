#if !defined(_H_IPPIMAGEPROCESSOR_) && defined(_IPP_)
#define _H_IPPIMAGEPROCESSOR_

class IPPImageProcessor : public ImageProcessor
{
private:
	static IPPImageProcessor* instance;

public:
	static IPPImageProcessor* getInstance(void);

public:
	IPPImageProcessor(void);
	~IPPImageProcessor(void);

private:
	IPPImageProcessor(IPPImageProcessor&);

public:
	virtual bool swap(const uint8_t* srcBuffer, const Area& srcSize, uint8_t* dstBuffer, const Area& dstSize, const PixelType pixelType, const int* order) override;
	virtual bool convert(PixelType srcPixelType, const uint8_t* srcBuffer, const Area& srcSize, PixelType dstPixelType, uint8_t* dstBuffer, const Area& dstSize) override;
	virtual bool scale(PixelType pixelType, const uint8_t* srcBuffer, size_t srcBufferLength, const Area& srcSize, uint8_t* dstBuffer, size_t dstBufferLength, const Area& dstSize) override;
	virtual bool value(PixelType pixelType, uint8_t* srcBuffer, size_t srcBufferLength, const Area& srcSize, int channelIndex, int value) override;
};

#endif