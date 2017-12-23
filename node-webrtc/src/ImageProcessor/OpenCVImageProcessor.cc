#include "../includes.h"
#if defined(_OPENCV_PREBUILT_) && !defined(_IPP_)

OpenCVImageProcessor* OpenCVImageProcessor::instance = nullptr;
OpenCVImageProcessor* OpenCVImageProcessor::getInstance()
{
	if (OpenCVImageProcessor::instance == nullptr)
		OpenCVImageProcessor::instance = new OpenCVImageProcessor();

	return OpenCVImageProcessor::instance;
}

OpenCVImageProcessor::OpenCVImageProcessor(void) {}
OpenCVImageProcessor::~OpenCVImageProcessor(void) {}

void __swap_4__(cv::Mat& src, cv::Mat& dst, const int order[4])
{
	const int mixOrder[] = {
		0, order[0],
		1, order[1],
		2, order[2],
		3, order[3]
	};

	cv::mixChannels(&src, 1, &dst, 1, mixOrder, 4);
}
void __swap_3__(cv::Mat& src, cv::Mat& dst, const int order[3])
{
	const int mixOrder[] = {
		0, order[0],
		1, order[1],
		2, order[2]
	};

	cv::mixChannels(&src, 1, &dst, 1, mixOrder, 3);
}

bool OpenCVImageProcessor::swap(const uint8_t* srcBuffer, const Area& srcSize, uint8_t* dstBuffer, const Area& dstSize, const PixelType pixelType, const int* order)
{
	int type = pixelType >= 1000 ? CV_8UC4 : CV_8UC3;
	cv::Mat srcImage(srcSize.getWidth(), srcSize.getHeight(), type, const_cast<uint8_t*>(srcBuffer));
	cv::Mat dstImage(dstSize.getWidth(), dstSize.getHeight(), type);

	if (type == CV_8UC4) 
	{
		const int mixOrder[] = {
			0, order[0],
			1, order[1],
			2, order[2],
			3, order[3]
		};

		cv::mixChannels(&srcImage, 1, &dstImage, 1, mixOrder, 4);
	}
	else if (type == CV_8UC3) 
	{
		const int mixOrder[] = {
			0, order[0],
			1, order[1],
			2, order[2]
		};

		cv::mixChannels(&srcImage, 1, &dstImage, 1, mixOrder, 3);
	}

	std::memcpy(dstBuffer, dstImage.data, dstSize.getWidth() * dstSize.getHeight() * (type == CV_8UC4 ? 4 : 3));
	return true;
}
bool OpenCVImageProcessor::convert(PixelType srcPixelType, const uint8_t* srcBuffer, const Area& srcSize, PixelType dstPixelType, uint8_t* dstBuffer, const Area& dstSize)
{
	int srcType = srcPixelType >= 1000 ? CV_8UC4 : CV_8UC3;
	int dstType = dstPixelType >= 1000 ? CV_8UC4 : CV_8UC3;

	cv::Mat srcImage(srcSize.getWidth(), srcSize.getHeight(), srcType, const_cast<uint8_t*>(srcBuffer));
	cv::Mat dstImage(dstSize.getWidth(), dstSize.getHeight(), dstType);

	int cvtCode = 0;
	switch (srcPixelType | dstPixelType) 
	{
	case RGB24 | RGBA32:
		cvtCode = srcPixelType == RGB24 ? CV_RGB2RGBA : CV_RGBA2RGB;
		break;

	case RGB24 | BGR24:
		cvtCode = CV_BGR2RGB;

	case RGBA32 | ARGB32:
		{
			int order[] = { 3, 2, 1, 0 };
			__swap_4__(srcImage, dstImage, order);
		}
		return true;

	default:
		return true;
	}

	cv::cvtColor(srcImage, dstImage, cvtCode);
	std::memcpy(dstBuffer, dstImage.data, dstSize.getWidth() * dstSize.getHeight() * (dstType == CV_8UC4 ? 4 : 3));
	return true;
}
bool OpenCVImageProcessor::scale(PixelType pixelType, const uint8_t* srcBuffer, size_t srcBufferLength, const Area& srcSize, uint8_t* dstBuffer, size_t dstBufferLength, const Area& dstSize)
{
	int type = pixelType >= 1000 ? CV_8UC4 : CV_8UC3;

	cv::Mat srcImage(srcSize.getWidth(), srcSize.getHeight(), type, const_cast<uint8_t*>(srcBuffer));
	cv::Mat dstImage(dstSize.getWidth(), dstSize.getHeight(), type);

	cv::resize(srcImage, dstImage, cv::Size(dstSize.getWidth(), dstSize.getHeight()), 0, 0, cv::INTER_NEAREST);

	std::memcpy(dstBuffer, dstImage.data, dstSize.getWidth() * dstSize.getHeight() * (type == CV_8UC4 ? 4 : 3));
	return true;
}
bool OpenCVImageProcessor::value(PixelType pixelType, uint8_t* srcBuffer, size_t srcBufferLength, const Area& srcSize, int channelIndex, int value)
{
	int type = pixelType >= 1000 ? CV_8UC4 : CV_8UC3;
	cv::Mat image(srcSize.getWidth(), srcSize.getHeight(), type, const_cast<uint8_t*>(srcBuffer));

	if (image.isContinuous() == true)
		image.reshape(1, image.rows * image.cols).col(channelIndex).setTo(cv::Scalar(value));
	else {
		for (unsigned int i = 0, j = static_cast<unsigned int>(image.rows); i < j; ++i)
			image.row(i).reshape(1, image.cols).col(channelIndex).setTo(cv::Scalar(value));
	}

	std::memcpy(srcBuffer, image.data, srcSize.getWidth() * srcSize.getHeight() * (type == CV_8UC4 ? 4 : 3));
	return true;
}

#endif