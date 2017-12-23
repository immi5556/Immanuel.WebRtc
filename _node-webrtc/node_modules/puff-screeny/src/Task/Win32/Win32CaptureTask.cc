#include "../../includes.h"

Win32CaptureTask::Win32CaptureTask(nbind::cbFunction& callback, nbind::Buffer& buffer, float scaleFactor, int fps, SharableTarget* target)
	: CaptureTask(callback, buffer, scaleFactor, fps, target), capturer(nullptr)
{
	if (!win32::isDXGISupported())
		capturer = GdiPlusCapturer::getInstance();
	else
		capturer = DXGICapturer::getInstance();
}

Win32CaptureTask::~Win32CaptureTask(void) {}

void Win32CaptureTask::takeScreenshot(std::uint8_t*& buffer, std::size_t& length)
{
	const SharableTarget* target = this->getTarget();
	PixelType pixelType;

	this->capturer->capture(target, &buffer, &length, &pixelType, this->getOriginalArea());
}