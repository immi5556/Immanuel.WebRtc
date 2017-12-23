#ifndef _H_WIN32SCREENSHOTTASK_
#define _H_WIN32SCREENSHOTTASK_

class Win32ScreenshotTask : public ScreenshotTask
{
public:
	Win32ScreenshotTask(nbind::cbFunction& callback_, nbind::Buffer& buffer, float scaleFactor, SharableTarget* target);
	~Win32ScreenshotTask(void);

private:
	Win32ScreenshotTask(Win32ScreenshotTask&) = delete;

public:
	void takeScreenshot(std::uint8_t*& buffer, std::size_t& length) override;

private:
	Capturer* capturer;
};

#endif
