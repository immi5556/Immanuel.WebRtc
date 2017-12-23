#ifndef _H_WIN32SCREEN_
#define _H_WIN32SCREEN_

class Win32Screen : public Screen
{
public:
	Win32Screen(HMONITOR targetHandle);
	Win32Screen(HDC deviceContext, const Area& area);
	Win32Screen(Win32Screen&) = delete;
	~Win32Screen(void);

public:
	virtual std::uint64_t getUniqueId(void) const override;
	virtual const Area& getArea(int dummy = 0) const override;
	virtual Area& getArea(void) override;
	
public:
	HMONITOR getHandle(void) const { return this->monitorHandle; }

public:
	HDC getDC(void) const { return this->deviceContext; }
	HDC getCaptureDC(void) const { return this->captureContext; }
	HBITMAP getCaptureBitmap(void) const { return this->captureBitmap; }

public:
	virtual bool startCapture(nbind::Buffer buffer, float scaleFactor, int fps, nbind::cbFunction& callback) override;
	virtual void stopCapture(nbind::cbFunction& callback) override;
	virtual void takeScreenshot(nbind::Buffer buffer, float scaleFactor, nbind::cbFunction& callback) override;

public:
	virtual void release(void) override;

private:
	// properties that used for GDI capture.
	HDC deviceContext;
	HDC captureContext;
	HBITMAP captureBitmap;

	// used for DXGI capture.
	HMONITOR monitorHandle;

	// general
	Area area;
	Win32CaptureTask* captureTask;
};

#endif