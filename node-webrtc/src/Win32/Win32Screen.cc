#include "../includes.h"

Win32Screen::Win32Screen(HMONITOR targetHandle)
	: deviceContext(nullptr), captureContext(nullptr), captureBitmap(nullptr), monitorHandle(targetHandle), captureTask(nullptr)
{
	MONITORINFOEXA monitorInformation;
	monitorInformation.cbSize = sizeof(MONITORINFOEX);

	GetMonitorInfoA(targetHandle, &monitorInformation);

	this->area.top = monitorInformation.rcMonitor.top;
	this->area.left = monitorInformation.rcMonitor.left;
	this->area.bottom = monitorInformation.rcMonitor.bottom;
	this->area.right = monitorInformation.rcMonitor.right;
}

Win32Screen::Win32Screen(HDC deviceContext, const Area& area)
	: deviceContext(deviceContext), captureContext(nullptr), captureBitmap(nullptr), monitorHandle(nullptr), area(area), captureTask(nullptr)
{
	this->deviceContext = deviceContext;
	this->captureContext = CreateCompatibleDC(deviceContext);
	this->captureBitmap = CreateCompatibleBitmap(deviceContext, area.getWidth(), area.getHeight());
}

Win32Screen::~Win32Screen(void)
{
	if (this->deviceContext != nullptr) {
		DeleteObject(this->captureBitmap);
		DeleteDC(this->captureContext);
	}
}

std::uint64_t Win32Screen::getUniqueId() const
{
	return reinterpret_cast<std::uint64_t>(this);
}

const Area& Win32Screen::getArea(int dummy) const
{
	return this->area;
}
Area& Win32Screen::getArea() 
{
	return this->area;
}

bool Win32Screen::startCapture(nbind::Buffer buffer, float scaleFactor, int fps, nbind::cbFunction& callback)
{
	if (scaleFactor == 0.0f)
		return false;

	if (this->captureTask != nullptr) {
		if (!this->captureTask->isFinished()) {
			return false;
		}
	}

	this->captureTask = new Win32CaptureTask(callback, buffer, scaleFactor, fps, this);
	Nan::AsyncQueueWorker(captureTask);
	return true;
}
void Win32Screen::stopCapture(nbind::cbFunction& callback)
{
	if (this->captureTask != nullptr) {
		this->captureTask->setFinishedCallback(callback);
		this->captureTask->stop();
		this->captureTask = nullptr;
	}
}
void Win32Screen::takeScreenshot(nbind::Buffer buffer, float scaleFactor, nbind::cbFunction& callback)
{
	if (scaleFactor == 0.0f)
		return;

	Nan::AsyncQueueWorker(new Win32ScreenshotTask(callback, buffer, scaleFactor, this));
}
void Win32Screen::release(void)
{
	delete this;
}

#include "nbind/nbind.h"

#if defined(BUILDING_NODE_EXTENSION)
NBIND_CLASS(Win32Screen)
{
	multimethod(getArea, args(), "getArea");
	method(getUniqueId);
	method(startCapture);
	method(stopCapture);
	method(takeScreenshot);
}
#endif