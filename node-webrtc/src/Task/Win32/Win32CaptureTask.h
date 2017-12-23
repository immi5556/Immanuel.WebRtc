#ifndef _H_WIN32CAPTURETASK_
#define _H_WIN32CAPTURETASK_

class Win32CaptureTask : public CaptureTask
{
public:
	Win32CaptureTask(nbind::cbFunction& callback, nbind::Buffer& buffer, float scaleFactor, int fps, SharableTarget* target);
	~Win32CaptureTask(void);

private:
	Win32CaptureTask(Win32CaptureTask&) = delete;

private:
	void takeScreenshot(std::uint8_t*& buffer, std::size_t& length) override;

private:
	Capturer* capturer;
};

#endif