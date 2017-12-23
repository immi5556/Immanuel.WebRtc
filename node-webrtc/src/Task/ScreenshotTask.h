#ifndef _H_SCREENSHOTTASK_
#define _H_SCREENSHOTTASK_

class ScreenshotTask : public Nan::AsyncWorker
{
protected:
	ScreenshotTask(nbind::cbFunction& callback_, nbind::Buffer& buffer, float scaleFactor, SharableTarget* target);
	~ScreenshotTask(void);

private:
	ScreenshotTask(ScreenshotTask&) = delete;

public:
	virtual void takeScreenshot(std::uint8_t*& buffer, std::size_t& length) = 0;
	virtual void preprocessScreenshot(std::uint8_t* buffer, std::size_t length);

protected:
	const SharableTarget* getTarget(void) const;
	const Area& getResizedArea(void) const;
	const Area& getOriginalArea(void) const;

private:
	void Execute() override;
	void HandleOKCallback() override;

private:
	SharableTarget* target;

	std::uint8_t* originalBuffer, *resizeBuffer;
	std::size_t originalBufferLength, resizeBufferLength;

	nbind::Buffer buffer;

	const Area& originalResolution;
	Area targetResolution;
	float scaleFactor;
};

#endif