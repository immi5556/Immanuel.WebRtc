#ifndef _H_WIN32WINDOW_
#define _H_WIN32WINDOW_

class Win32Window : public Window
{
private:
	static HWINEVENTHOOK hookHandle;
	static bool hookEnabled;
	static std::thread* hookThread;
	static std::unordered_map<HWND, Win32Window*> instances;

private:
	static void CALLBACK hookProcedure(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD idEventThread, DWORD dwmsEventTime);
	static void hookThreadFunction();

public:
	static puff::type_helper::window_t* create(HWND targetHandle);

private:
	Win32Window(void);
	explicit Win32Window(HWND targetHandle);
	Win32Window(Win32Window&);
	~Win32Window(void);

public:
	HWND getHandle(void) const { return this->targetHandle; }
	HBRUSH getBackground(void) const { return this->background; }

public:
	virtual std::uint64_t getUniqueId(void) const override;
	virtual const Area& getArea(int dummy = 0) const override;
	virtual Area& getArea(void) override;
	virtual puff::string_t getTitle(void) override;
	virtual bool startCapture(nbind::Buffer buffer, float scaleFactor, int fps, nbind::cbFunction& callback) override;
	virtual void stopCapture(nbind::cbFunction& callback) override;
	virtual void takeScreenshot(nbind::Buffer buffer, float scaleFactor, nbind::cbFunction& callback) override;

public:
	virtual void release(void) override;

private:
	Area area;
	HWND targetHandle;
	Win32CaptureTask* captureTask;
	HBRUSH background;
};

#endif
