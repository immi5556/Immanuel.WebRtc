#include "../../includes.h"

Win32ScreenshotTask::Win32ScreenshotTask(nbind::cbFunction& callback_, nbind::Buffer& buffer, float scaleFactor, SharableTarget* target) :
	ScreenshotTask(callback_, buffer, scaleFactor, target)
{
	if (!win32::isDXGISupported())
		capturer = GdiPlusCapturer::getInstance();
	else
		capturer = DXGICapturer::getInstance();
}
Win32ScreenshotTask::~Win32ScreenshotTask(void) {}

void Win32ScreenshotTask::takeScreenshot(std::uint8_t*& buffer, std::size_t& length)
{
	PixelType pixelType;
	const SharableTarget* target = this->getTarget();
	if (target->isScreen())
		capturer->capture(target, &buffer, &length, &pixelType, this->getOriginalArea());
	else
		GdiPlusCapturer::getInstance()->capture(target, &buffer, &length, &pixelType, this->getOriginalArea());
}