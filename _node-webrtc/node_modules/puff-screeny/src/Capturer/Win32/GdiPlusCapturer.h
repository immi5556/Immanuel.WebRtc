#ifndef _H_GDIPLUSCAPTURER_
#define _H_GDIPLUSCAPTURER_

class GdiPlusCapturer : public Capturer
{
private:
	static GdiPlusCapturer* instance;

public:
	static GdiPlusCapturer* getInstance(void);

private:
	GdiPlusCapturer(void);
	~GdiPlusCapturer(void);

private:
	GdiPlusCapturer(GdiPlusCapturer&) = delete;

public:
	virtual bool isHWAccelerated(void) const override;

private:
	bool captureWindow(const puff::type_helper::window_t* target, std::uint8_t** dstBuffer, std::size_t* byteWritten, PixelType* dstPixelType, const Area& originalArea);
	bool captureScreen(const puff::type_helper::screen_t* target, std::uint8_t** dstBuffer, std::size_t* byteWritten, PixelType* dstPixelType, const Area& originalArea);

public:
	virtual bool capture(const SharableTarget* target, std::uint8_t** dstBuffer, std::size_t* byteWritten, PixelType* dstPixelType, const Area& originalArea) override;

private:
	std::unordered_map<const puff::type_helper::window_t*, HDC> deviceContext;
	std::unordered_map<const puff::type_helper::window_t*, HDC> captureContext;
	std::unordered_map<const puff::type_helper::window_t*, HBITMAP> captureBitmap;

	ULONG_PTR token;
	Gdiplus::GdiplusStartupInput input;
	bool failed;

	uv_mutex_t mutex;
};

#endif