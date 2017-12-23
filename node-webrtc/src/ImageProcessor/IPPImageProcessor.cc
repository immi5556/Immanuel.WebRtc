#include "../includes.h"
#if defined(_IPP_)

#include <ipp.h>

IPPImageProcessor* IPPImageProcessor::instance = nullptr;

IPPImageProcessor* IPPImageProcessor::getInstance()
{
	if (IPPImageProcessor::instance == nullptr && ippInit() == IppStatus::ippStsNoErr)
		IPPImageProcessor::instance = new IPPImageProcessor();

	return IPPImageProcessor::instance;
}

IPPImageProcessor::IPPImageProcessor(void) {}
IPPImageProcessor::IPPImageProcessor(IPPImageProcessor&) {}
IPPImageProcessor::~IPPImageProcessor(void) {}

IppStatus _image_resize_nearest_(PixelType pixelType, const Ipp8u* pSrc, IppiSize srcSize, Ipp32s srcStep, Ipp8u* pDst, IppiSize dstSize, Ipp32s dstStep)
{
	IppiResizeSpec_32f* pSpec = nullptr;
	int specSize = 0, initSize = 0, bufSize = 0;
	Ipp8u* pBuffer = nullptr;
	Ipp8u* pInitBuf = nullptr;
	IppiPoint dstOffset = { 0, 0 };
	IppStatus status = ippStsNoErr;

	Ipp32u numChannels = 3;
	switch (pixelType)
	{
	case PixelType::ARGB32:
	case PixelType::RGBA32:
		numChannels = 4;
		break;

	case PixelType::RGB24:
		numChannels = 3;
		break;
	}

	// Spec and init buffer sizes
	status = ippiResizeGetSize_8u(srcSize, dstSize, ippNearest, 0, &specSize, &initSize);
	if (status != ippStsNoErr) 
		return status;

	// Memory allocation
	pInitBuf = ippsMalloc_8u(initSize);
	pSpec = reinterpret_cast<IppiResizeSpec_32f*>(ippsMalloc_8u(specSize));

	if (pInitBuf == nullptr || pSpec == nullptr)
	{
		ippsFree(pInitBuf);
		ippsFree(pSpec);
		return ippStsNoMemErr;
	}

	// Filter initialization
	status = ippiResizeNearestInit_8u(srcSize, dstSize, pSpec);
	ippsFree(pInitBuf);

	if (status != ippStsNoErr)
	{
		ippsFree(pSpec);
		return status;
	}

	// work buffer size
	status = ippiResizeGetBufferSize_8u(pSpec, dstSize, numChannels, &bufSize);
	if (status != ippStsNoErr)
	{
		ippsFree(pSpec);
		return status;
	}

	pBuffer = ippsMalloc_8u(bufSize);
	if (pBuffer == nullptr)
	{
		ippsFree(pSpec);
		return ippStsNoMemErr;
	}

	if (numChannels == 3)
		status = ippiResizeNearest_8u_C3R(pSrc, srcStep, pDst, dstStep, dstOffset, dstSize, pSpec, pBuffer);
	else if (numChannels == 4)
		status = ippiResizeNearest_8u_C4R(pSrc, srcStep, pDst, dstStep, dstOffset, dstSize, pSpec, pBuffer);

	ippsFree(pSpec);
	ippsFree(pBuffer);

	return status;
}
IppStatus _image_resize_lanzcos_(PixelType pixelType, const Ipp8u* pSrc, IppiSize srcSize, Ipp32s srcStep, Ipp8u* pDst, IppiSize dstSize, Ipp32s dstStep)
{
	IppiResizeSpec_32f* pSpec = 0;
	int specSize = 0, initSize = 0, bufSize = 0;
	Ipp8u* pBuffer = 0;
	Ipp8u* pInitBuf = 0;
	Ipp32u numChannels = 3;
	switch (pixelType)
	{
	case PixelType::ARGB32:
	case PixelType::RGBA32:
		numChannels = 4;
		break;

	case PixelType::RGB24:
		numChannels = 3;
		break;
	}
	IppiPoint dstOffset = { 0, 0 };
	IppStatus status = ippStsNoErr;
	IppiBorderType border = ippBorderRepl;

	/* Spec and init buffer sizes */
	status = ippiResizeGetSize_8u(srcSize, dstSize, ippLanczos, 0, &specSize, &initSize);

	if (status != ippStsNoErr) return status;

	/* Memory allocation */
	pInitBuf = ippsMalloc_8u(initSize);
	pSpec = (IppiResizeSpec_32f*)ippsMalloc_8u(specSize);

	if (pInitBuf == NULL || pSpec == NULL)
	{
		ippsFree(pInitBuf);
		ippsFree(pSpec);
		return ippStsNoMemErr;
	}

	/* Filter initialization */
	status = ippiResizeLanczosInit_8u(srcSize, dstSize, 3, pSpec, pInitBuf);
	ippsFree(pInitBuf);

	if (status != ippStsNoErr)
	{
		ippsFree(pSpec);
		return status;
	}

	/* work buffer size */
	status = ippiResizeGetBufferSize_8u(pSpec, dstSize, numChannels, &bufSize);
	if (status != ippStsNoErr)
	{
		ippsFree(pSpec);
		return status;
	}

	pBuffer = ippsMalloc_8u(bufSize);
	if (pBuffer == NULL)
	{
		ippsFree(pSpec);
		return ippStsNoMemErr;
	}

	/* Resize processing */
	status = ippiResizeLanczos_8u_C4R(pSrc, srcStep, pDst, dstStep, dstOffset, dstSize, border, 0, pSpec, pBuffer);

	ippsFree(pSpec);
	ippsFree(pBuffer);

	return status;
}

bool IPPImageProcessor::scale(PixelType pixelType, const uint8_t* srcBuffer, size_t srcBufferLength, const Area& srcSize, uint8_t* dstBuffer, size_t dstBufferLength, const Area& dstSize)
{
	int channelCount = 0;
	if (pixelType == PixelType::RGBA32 || pixelType == PixelType::ARGB32)
		channelCount = 4;
	else if (pixelType == PixelType::RGB24)
		channelCount = 3;

	if (_image_resize_lanzcos_(pixelType, srcBuffer, { srcSize.getWidth(), srcSize.getHeight() }, srcSize.getWidth() * channelCount,
		reinterpret_cast<Ipp8u*>(dstBuffer), { dstSize.getWidth(), dstSize.getHeight() }, dstSize.getWidth() * channelCount) != IppStatus::ippStsNoErr)
		return false;

	return true;
}
bool IPPImageProcessor::value(PixelType pixelType, uint8_t* srcBuffer, size_t srcBufferLength, const Area& srcSize, int channelIndex, int value)
{
	int channelCount = 0;
	if (pixelType == PixelType::RGBA32 || pixelType == PixelType::ARGB32)
		channelCount = 4;
	else if (pixelType == PixelType::RGB24)
		channelCount = 3;

	if (channelCount == 4)
		return (ippiSet_8u_C4CR(value, srcBuffer + channelIndex, srcSize.getWidth() * channelCount, { srcSize.getWidth(), srcSize.getHeight() }) == IppStatus::ippStsNoErr);
	else if (channelCount == 3)
		return (ippiSet_8u_C3CR(value, srcBuffer + channelIndex, srcSize.getWidth() * channelCount, { srcSize.getWidth(), srcSize.getHeight() }) == IppStatus::ippStsNoErr);
}
bool IPPImageProcessor::swap(const uint8_t* srcBuffer, const Area& srcSize, uint8_t* dstBuffer, const Area& dstSize, const PixelType pixelType, const int* order)
{
	return (ippiSwapChannels_8u_C4R(srcBuffer, srcSize.getWidth() * 4, dstBuffer, dstSize.getWidth() * 4, { srcSize.getWidth(), srcSize.getHeight() }, order) == IppStatus::ippStsNoErr);
}
bool IPPImageProcessor::convert(PixelType srcPixelType, const uint8_t* srcBuffer, const Area& srcSize, PixelType dstPixelType, uint8_t* dstBuffer, const Area& dstSize)
{
	int dstChannels = dstPixelType >= 1000 ? 4 : 3, srcChannels = srcPixelType >= 1000 ? 4 : 3;
	
	IppStatus ret = IppStatus::ippStsNoErr;
	if (dstChannels == 3 && srcChannels == 4) {
		// Default RGBA => RGB
		int order[3] = { 0, 1, 2 };
		if (srcPixelType == PixelType::ARGB32) {
			if (dstPixelType == PixelType::RGB24) {
				order[0] = 1;
				order[1] = 2;
				order[2] = 3;
			}
		}

		ret = ippiSwapChannels_8u_C4C3R(
			srcBuffer, srcChannels * srcSize.getWidth(),
			dstBuffer, dstChannels * dstSize.getWidth(),
			{ srcSize.getWidth(), srcSize.getHeight() },
			order
		);
	} else if (dstChannels == 4 && srcChannels == 3) {
		// Default RGB => RGBA
		int order[4] = { 0, 1, 2, 3 };
		if (srcPixelType == PixelType::RGB24) {
			if (dstPixelType == PixelType::ARGB32) {
				order[0] = 3;
				order[1] = 0;
				order[2] = 1;
				order[3] = 2;
			}
		}

		ret = ippiSwapChannels_8u_C3C4R(
			srcBuffer, srcChannels * srcSize.getWidth(),
			dstBuffer, dstChannels * dstSize.getWidth(),
			{ srcSize.getWidth(), srcSize.getHeight() },
			order, 0xFF
		);
	} else if (dstChannels == srcChannels) {
		int order[4] = { 0, };
		if (srcPixelType == PixelType::ARGB32 && dstPixelType == PixelType::RGBA32) {
			ippiCopy_8u_AC4R(srcBuffer, srcChannels * srcSize.getWidth(), dstBuffer, dstChannels * dstSize.getWidth(), { srcSize.getWidth(), srcSize.getHeight() });
			order[0] = 1;
			order[1] = 2;
			order[2] = 3;
			order[3] = 0;

			return true;
		}
		ret = ippiSwapChannels_8u_C4R(
			srcBuffer, srcChannels * srcSize.getWidth(),
			dstBuffer, dstChannels * dstSize.getWidth(),
			{ srcSize.getWidth(), srcSize.getHeight() },
			order
		);
	}

	return (ret == IppStatus::ippStsNoErr);
}

#endif
