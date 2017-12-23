#include "../../includes.h"

#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT    0x00000002
#endif

GdiPlusCapturer* GdiPlusCapturer::instance = nullptr;

GdiPlusCapturer* GdiPlusCapturer::getInstance()
{
	if (GdiPlusCapturer::instance == nullptr)
	{
		GdiPlusCapturer::instance = new GdiPlusCapturer();
	}

	return GdiPlusCapturer::instance;
}

GdiPlusCapturer::GdiPlusCapturer(void)
	: token(0), failed(false)
{
	uv_mutex_init(&this->mutex);
}
GdiPlusCapturer::~GdiPlusCapturer(void)
{
	uv_mutex_destroy(&this->mutex);
}

bool GdiPlusCapturer::isHWAccelerated() const
{
	return false;
}

bool GdiPlusCapturer::captureWindow(const puff::type_helper::window_t* target, std::uint8_t** dstBuffer, std::size_t* byteWritten, PixelType* dstPixelType, const Area& originalArea)
{
	const Area& windowArea   = target->getArea();
	HDC       deviceContext  = nullptr;
	HDC       captureContext = nullptr;
	HBITMAP   captureBitmap  = nullptr;
	if (this->deviceContext.find(target) == this->deviceContext.end())
	{
		this->deviceContext[target]  = deviceContext  = GetWindowDC(target->getHandle());
		this->captureContext[target] = captureContext = CreateCompatibleDC(deviceContext);
		this->captureBitmap[target]  = captureBitmap  = CreateCompatibleBitmap(deviceContext, windowArea.getWidth(), windowArea.getHeight());
	}
	else
	{
		deviceContext  = this->deviceContext[target];
		captureContext = this->captureContext[target];
		captureBitmap  = this->captureBitmap[target];
	}

	//
	// Capture window using PrintWindow() function.
	//
	// If the running os is Windows 8 or later, we will use only this way because
	// DWM-related functions are not having any effect since Windows 8 and Some
	// applications return just black image when using BitBlt on Windows 8 or higher.  
	//
	// In Windows 7, PrintWindow() function will be used only if Aero is disabled.
	//
	// Note: PrintWindow() is slower than BitBlt().
	//
	HGDIOBJ originalBitmap = SelectObject(captureContext, captureBitmap);
	RECT    windowRect;
	SetRect(&windowRect, windowArea.left, windowArea.top, windowArea.right, windowArea.bottom);
	FillRect(captureContext, &windowRect, target->getBackground());

	BOOL result = FALSE;
	if ((win32::getWindowsVersion() <= win32::Windows7 && win32::isAeroEnabled() == false) || true)
	{
		UINT flags = win32::getWindowsVersion() >= win32::Windows8 ? PW_RENDERFULLCONTENT : 0;
		result = PrintWindow(target->getHandle(), captureContext, flags);
	}

	if (result == FALSE)
	{
		result = BitBlt(captureContext, 0, 0, windowArea.getWidth(), windowArea.getHeight(), deviceContext, 0, 0, SRCCOPY | CAPTUREBLT);
	}

	if (result == FALSE)
	{
		SelectObject(captureContext, originalBitmap);
		return false;
	}

	BITMAPINFOHEADER bitmapInfo = {0,};
	bitmapInfo.biSize        = sizeof(BITMAPINFOHEADER);
	bitmapInfo.biWidth       = windowArea.getWidth();
	bitmapInfo.biHeight      = -windowArea.getHeight();
	bitmapInfo.biPlanes      = 1;
	bitmapInfo.biBitCount    = 32;
	bitmapInfo.biCompression = BI_RGB;
	bitmapInfo.biSizeImage   = static_cast<DWORD>(((windowArea.getWidth() * bitmapInfo.biBitCount + 31) / 32) * 4 * windowArea.getHeight());

	*dstPixelType = PixelType::ARGB32;

	GetDIBits(deviceContext, captureBitmap, 0, static_cast<UINT>(windowArea.getHeight()), *dstBuffer, reinterpret_cast<BITMAPINFO*>(&bitmapInfo), DIB_RGB_COLORS);
	SelectObject(captureContext, originalBitmap);
	return true;
}
bool GdiPlusCapturer::captureScreen(const puff::type_helper::screen_t* target, std::uint8_t** dstBuffer, std::size_t* byteWritten, PixelType* dstPixelType, const Area& originalArea)
{
	SCOPED_LOCK(mutex);

	HDC     deviceContext  = target->getDC();
	HDC     captureDC      = target->getCaptureDC();
	HBITMAP captureBitmap  = target->getCaptureBitmap();
	HGDIOBJ originalBitmap = SelectObject(captureDC, captureBitmap);

	const Area& area = target->getArea();

	BitBlt(captureDC, 0, 0, area.getWidth(), area.getHeight(), deviceContext, area.left, area.top, SRCCOPY | CAPTUREBLT);

	BITMAPINFOHEADER bitmapInfo = {0,};
	bitmapInfo.biSize        = sizeof(BITMAPINFOHEADER);
	bitmapInfo.biWidth       = area.getWidth();
	bitmapInfo.biHeight      = -area.getHeight();
	bitmapInfo.biPlanes      = 1;
	bitmapInfo.biBitCount    = 32;
	bitmapInfo.biCompression = BI_RGB;
	bitmapInfo.biSizeImage   = ((area.getWidth() * bitmapInfo.biBitCount + 31) / 32) * 4 * area.getHeight();

	*dstPixelType = PixelType::ARGB32;

	GetDIBits(deviceContext, captureBitmap, 0, area.getHeight(), *dstBuffer, reinterpret_cast<BITMAPINFO*>(&bitmapInfo), DIB_RGB_COLORS);
	SelectObject(captureDC, originalBitmap);

	return true;
}

bool GdiPlusCapturer::capture(const SharableTarget* target, std::uint8_t** dstBuffer, std::size_t* byteWritten, PixelType* dstPixelType, const Area& originalArea)
{
	switch (target->getType())
	{
	case SharableTargetType::window:
		return this->captureWindow(reinterpret_cast<const Win32Window*>(target), dstBuffer, byteWritten, dstPixelType, originalArea);

	case SharableTargetType::screen:
		return this->captureScreen(reinterpret_cast<const Win32Screen*>(target), dstBuffer, byteWritten, dstPixelType, originalArea);

	default:
		return false;
	}
}
